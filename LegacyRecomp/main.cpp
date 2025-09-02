#include <stdafx.h>
#include <cpuid.h>
#include <cpu/guest_thread.h>
#include <gpu/video.h>
#include <kernel/function.h>
#include <kernel/memory.h>
#include <kernel/heap.h>
#include <kernel/xam.h>
#include <kernel/io/file_system.h>
#include <file.h>
#include <xex.h>
#include <apu/audio.h>
#include <hid/hid.h>
#include <config.h>
#include <kernel/xdbf.h>
#include <os/logger.h>
#include <os/process.h>
#include <os/registry.h>
#include <ui/game_window.h>
//#include <mod/mod_loader.h>
#include <preload_executable.h>

#ifdef _WIN32
#include <timeapi.h>
#endif

#if defined(_WIN32) && defined(UNLEASHED_RECOMP_D3D12)
static std::array<std::string_view, 3> g_D3D12RequiredModules =
{
    "D3D12/D3D12Core.dll",
    "dxcompiler.dll",
    "dxil.dll"
};
#endif

const size_t XMAIOBegin = 0x7FEA0000;
const size_t XMAIOEnd = XMAIOBegin + 0x0000FFFF;

Memory g_memory;
Heap g_userHeap;
XDBFWrapper g_xdbfWrapper;
std::unordered_map<uint16_t, GuestTexture*> g_xdbfTextureCache;

void HostStartup()
{
#ifdef _WIN32
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif

    hid::Init();
}

// Name inspired from nt's entry point
void KiSystemStartup()
{
    if (g_memory.base == nullptr)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, GameWindow::GetTitle(), "System_MemoryAllocationFailed", GameWindow::s_pWindow);
        std::_Exit(1);
    }

    g_userHeap.Init();

    const auto gameContent = XamMakeContent(XCONTENTTYPE_RESERVED, "Game");
    const auto updateContent = XamMakeContent(XCONTENTTYPE_RESERVED, "Update");
    //XamRegisterContent(gameContent, GAME_INSTALL_DIRECTORY "/game");
    //XamRegisterContent(updateContent, GAME_INSTALL_DIRECTORY "/update");
    //
    //const auto saveFilePath = GetSaveFilePath(true);
    //bool saveFileExists = std::filesystem::exists(saveFilePath);

    //if (!saveFileExists)
    //{
    //    // Copy base save data to modded save as fallback.
    //    std::error_code ec;
    //    std::filesystem::create_directories(saveFilePath.parent_path(), ec);
    //
    //    if (!ec)
    //    {
    //        std::filesystem::copy_file(GetSaveFilePath(false), saveFilePath, ec);
    //        saveFileExists = !ec;
    //    }
    //}
    //
    //if (saveFileExists)
    //{
    //    std::u8string savePathU8 = saveFilePath.parent_path().u8string();
    //    XamRegisterContent(XamMakeContent(XCONTENTTYPE_SAVEDATA, "SYS-DATA"), (const char*)(savePathU8.c_str()));
    //}

    // Mount game
    XamContentCreateEx(0, "game", &gameContent, OPEN_EXISTING, nullptr, nullptr, 0, 0, nullptr);
    XamContentCreateEx(0, "update", &updateContent, OPEN_EXISTING, nullptr, nullptr, 0, 0, nullptr);

    // OS mounts game data to D:
    XamContentCreateEx(0, "D", &gameContent, OPEN_EXISTING, nullptr, nullptr, 0, 0, nullptr);

    std::error_code ec;
    //for (auto& file : std::filesystem::directory_iterator(GAME_INSTALL_DIRECTORY "/dlc", ec))
    //{
    //    if (file.is_directory())
    //    {
    //        std::u8string fileNameU8 = file.path().filename().u8string();
    //        std::u8string filePathU8 = file.path().u8string();
    //        XamRegisterContent(XamMakeContent(XCONTENTTYPE_DLC, (const char*)(fileNameU8.c_str())), (const char*)(filePathU8.c_str()));
    //    }
    //}

    XAudioInitializeSystem();
}

uint32_t LdrLoadModule(const std::filesystem::path &path)
{
    auto loadResult = LoadFile(path);
    if (loadResult.empty())
    {
        assert("Failed to load module" && false);
        return 0;
    }

    auto* header = reinterpret_cast<const Xex2Header*>(loadResult.data());
    auto* security = reinterpret_cast<const Xex2SecurityInfo*>(loadResult.data() + header->securityOffset);
    const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_FILE_FORMAT_INFO));
    auto entry = *reinterpret_cast<const uint32_t*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_ENTRY_POINT));
    ByteSwapInplace(entry);

    auto srcData = loadResult.data() + header->headerSize;
    auto destData = reinterpret_cast<uint8_t*>(g_memory.Translate(security->loadAddress));

    if (fileFormatInfo->compressionType == XEX_COMPRESSION_NONE)
    {
        memcpy(destData, srcData, security->imageSize);
    }
    else if (fileFormatInfo->compressionType == XEX_COMPRESSION_BASIC)
    {
        auto* blocks = reinterpret_cast<const Xex2FileBasicCompressionBlock*>(fileFormatInfo + 1);
        const size_t numBlocks = (fileFormatInfo->infoSize / sizeof(Xex2FileBasicCompressionInfo)) - 1;

        for (size_t i = 0; i < numBlocks; i++)
        {
            memcpy(destData, srcData, blocks[i].dataSize);

            srcData += blocks[i].dataSize;
            destData += blocks[i].dataSize;

            memset(destData, 0, blocks[i].zeroSize);
            destData += blocks[i].zeroSize;
        }
    }
    else
    {
        assert(false && "Unknown compression type.");
    }

    auto res = reinterpret_cast<const Xex2ResourceInfo*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_RESOURCE_INFO));

    g_xdbfWrapper = XDBFWrapper((uint8_t*)g_memory.Translate(res->offset.get()), res->sizeOfData);

    return entry;
}

//__attribute__((constructor(101), target("no-avx,no-avx2"), noinline))
void init()
{
#ifdef __x86_64__
    uint32_t eax, ebx, ecx, edx;

    // Execute CPUID for processor info and feature bits.
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);

    // Check for AVX support.
    if ((ecx & (1 << 28)) == 0)
    {
        printf("[*] CPU does not support the AVX instruction set.\n");

#ifdef _WIN32
        MessageBoxA(nullptr, "Your CPU does not meet the minimum system requirements.", "Unleashed Recompiled", MB_ICONERROR);
#endif

        std::_Exit(1);
    }
#endif
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
    timeBeginPeriod(1);
#endif

    os::process::CheckConsole();

    if (!os::registry::Init())
        LOGN_WARNING("OS does not support registry.");

    os::logger::Init();

    PreloadContext preloadContext;
    preloadContext.PreloadExecutable();

    bool forceInstaller = false;
    bool forceDLCInstaller = false;
    bool useDefaultWorkingDirectory = false;
    bool forceInstallationCheck = false;
    bool graphicsApiRetry = false;
    const char *sdlVideoDriver = nullptr;

    for (uint32_t i = 1; i < argc; i++)
    {
        forceInstaller = forceInstaller || (strcmp(argv[i], "--install") == 0);
        forceDLCInstaller = forceDLCInstaller || (strcmp(argv[i], "--install-dlc") == 0);
        useDefaultWorkingDirectory = useDefaultWorkingDirectory || (strcmp(argv[i], "--use-cwd") == 0);
        forceInstallationCheck = forceInstallationCheck || (strcmp(argv[i], "--install-check") == 0);
        graphicsApiRetry = graphicsApiRetry || (strcmp(argv[i], "--graphics-api-retry") == 0);

        if (strcmp(argv[i], "--sdl-video-driver") == 0)
        {
            if ((i + 1) < argc)
                sdlVideoDriver = argv[++i];
            else
                LOGN_WARNING("No argument was specified for --sdl-video-driver. Option will be ignored.");
        }
    }

    if (!useDefaultWorkingDirectory)
    {
        // Set the current working directory to the executable's path.
        std::error_code ec;
        std::filesystem::current_path(os::process::GetExecutablePath().parent_path(), ec);
    }

    Config::Load();

    HostStartup();

  

    if (!Video::CreateHostDevice(sdlVideoDriver, graphicsApiRetry))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, GameWindow::GetTitle(), "Video_BackendError", GameWindow::s_pWindow);
        std::_Exit(1);
    }

    

    KiSystemStartup();


    // TEMP, we would want to load it dinamicaly, idk a config or something
    std::filesystem::path modulePath = "F:/priv/LCE_RECOMP/private/tu2.xex";

    //
    // TODO Note:
    // Support encrypted mode, so you don't need to decrypt the xex yourself.
    // i think unleashed did that with the installer, but we yeeted their installer lol
    uint32_t entry = LdrLoadModule(modulePath);

    Video::StartPipelinePrecompilation();

    GuestThread::Start({ entry, 0, 0 });

    return 0;
}


GUEST_FUNCTION_STUB_P(sub_82601FE0); // NuiInit stuff (kinect)

GUEST_FUNCTION_STUB_P(__imp__vsprintf);
GUEST_FUNCTION_STUB_P(__imp___vsnprintf);
GUEST_FUNCTION_STUB_P(__imp__sprintf);
GUEST_FUNCTION_STUB_P(__imp___snprintf);
GUEST_FUNCTION_STUB_P(__imp___snwprintf);
GUEST_FUNCTION_STUB_P(__imp__vswprintf);
GUEST_FUNCTION_STUB_P(__imp___vscwprintf);
GUEST_FUNCTION_STUB_P(__imp__swprintf);
