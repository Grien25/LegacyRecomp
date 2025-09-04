# PPC_FUNC Documentation

This document catalogs all PPC_FUNC implementations across the Framework Recomp codebase, explaining their purpose and functionality.

## Overview

PPC_FUNC is a macro system used to hook PowerPC (PPC) functions in the Xbox 360 executable. It allows the runtime to intercept, modify, or replace original game functions with custom implementations.

### Macro Definitions

- `PPC_FUNC(name)` - Defines a PPC function hook
- `PPC_FUNC_IMPL(__imp__name)` - Defines the implementation pointer
- `GUEST_FUNCTION_HOOK(subroutine, function)` - Hooks a PPC function to a host function
- `GUEST_FUNCTION_STUB(subroutine)` - Creates a stub (empty) implementation

---

## app.cpp

### Commented Out PPC Functions

#### `sub_824EB490` (Application Constructor)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824EB490);
//PPC_FUNC(sub_824EB490)
//{
//    App::s_isInit = true;
//    App::s_isMissingDLC = !Installer::checkAllDLC(GetGamePath());
//    App::s_language = Config::Language;
//
//    SWA::SGlobals::Init();
//    Registry::Save();
//
//    __imp__sub_824EB490(ctx, base);
//}
```
**Purpose**: Application initialization hook for SWA (Sonic World Adventure). Sets up initial application state, checks for DLC, initializes global systems, and saves registry settings.

#### `sub_822C1130` (Application Update Loop)
```cpp
//PPC_FUNC_IMPL(__imp__sub_822C1130);
//PPC_FUNC(sub_822C1130)
//{
//    Video::WaitOnSwapChain();
//
//    // Correct small delta time errors.
//    if (Config::FPS >= FPS_MIN && Config::FPS < FPS_MAX)
//    {
//        double targetDeltaTime = 1.0 / Config::FPS;
//
//        if (abs(ctx.f1.f64 - targetDeltaTime) < 0.00001)
//            ctx.f1.f64 = targetDeltaTime;
//    }
//
//    App::s_deltaTime = ctx.f1.f64;
//    App::s_time += App::s_deltaTime;
//
//    // This function can also be called by the loading thread,
//    // which SDL does not like. To prevent the OS from thinking
//    // the process is unresponsive, we will flush while waiting
//    // for the pipelines to finish compiling in video.cpp.
//    if (std::this_thread::get_id() == g_mainThreadId)
//    {
//        SDL_PumpEvents();
//        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
//        GameWindow::Update();
//    }
//
//    AudioPatches::Update(App::s_deltaTime);
//    InspirePatches::Update();
//
//    // Apply subtitles option.
//    if (auto pApplicationDocument = SWA::CApplicationDocument::GetInstance())
//        pApplicationDocument->m_InspireSubtitles = Config::Subtitles;
//
//    if (Config::EnableEventCollisionDebugView)
//        *SWA::SGlobals::ms_IsTriggerRender = true;
//
//    if (Config::EnableGIMipLevelDebugView)
//        *SWA::SGlobals::ms_VisualizeLoadedLevel = true;
//
//    if (Config::EnableObjectCollisionDebugView)
//        *SWA::SGlobals::ms_IsObjectCollisionRender = true;
//
//    if (Config::EnableStageCollisionDebugView)
//        *SWA::SGlobals::ms_IsCollisionRender = true;
//
//    __imp__sub_822C1130(ctx, base);
//}
```
**Purpose**: Main application update loop hook. Handles frame timing correction, window event processing, audio updates, debug visualization toggles, and various system patches.

---

## function.h

### Core PPC Function System

#### `HostToGuestFunction` (Template Function)
```cpp
template<auto Func>
PPC_FUNC(HostToGuestFunction)
{
    using ret_t = decltype(std::apply(Func, function_args(Func)));

    auto args = function_args(Func);
    _translate_args_to_host<Func>(ctx, base, args);

    if constexpr (std::is_same_v<ret_t, void>)
    {
        std::apply(Func, args);
    }
    else
    {
        auto v = std::apply(Func, args);

        if constexpr (std::is_pointer<ret_t>())
        {
            if (v != nullptr)
            {
                ctx.r3.u64 = static_cast<uint32_t>(reinterpret_cast<size_t>(v) - reinterpret_cast<size_t>(base));
            }
            else
            {
                ctx.r3.u64 = 0;
            }
        }
        else if constexpr (is_precise_v<ret_t>)
        {
            ctx.f1.f64 = v;
        }
        else
        {
            ctx.r3.u64 = (uint64_t)v;
        }
    }
}
```
**Purpose**: Core template function that bridges host C++ functions with guest PPC functions. Translates arguments from PPC context to host function parameters and handles return value translation back to PPC registers.

#### `GUEST_FUNCTION_HOOK` Macro
```cpp
#define GUEST_FUNCTION_HOOK(subroutine, function) \
    PPC_FUNC(subroutine) { HostToGuestFunction<function>(ctx, base); }
```
**Purpose**: Macro that creates a PPC function hook that redirects to a host function implementation.

#### `GUEST_FUNCTION_STUB` Macro
```cpp
#define GUEST_FUNCTION_STUB(subroutine) \
    PPC_FUNC(subroutine) { }
```
**Purpose**: Macro that creates an empty stub implementation for PPC functions that don't need custom behavior.

#### `GUEST_FUNCTION_STUB_P` Macro
```cpp
#define GUEST_FUNCTION_STUB_P(subroutine) \
    PPC_FUNC(subroutine) { printf(#subroutine " is Stubbed\n"); }
```
**Purpose**: Macro that creates a stub with debug output to indicate when a function is called.

---

## misc_impl.cpp

### System Function Implementations

#### `sub_82BD4AE8` (sprintf Implementation)
```cpp
//PPC_FUNC(sub_82BD4AE8)
//{
//    sub_831B1630(ctx, base);
//}
```
**Purpose**: Hook for sprintf function that redirects to the actual sprintf implementation. Currently commented out.

### Host Function Hooks

#### Memory Functions
```cpp
GUEST_FUNCTION_HOOK(sub_831B0ED0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CCB98, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEAE0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEE04, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF2D0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF660, memcpy);
GUEST_FUNCTION_HOOK(sub_831B1358, memcpy);
GUEST_FUNCTION_HOOK(sub_831B5E00, memmove);
GUEST_FUNCTION_HOOK(sub_831B0BA0, memset);
GUEST_FUNCTION_HOOK(sub_831CCAA0, memset);
```
**Purpose**: Multiple memory function hooks that redirect PPC memcpy, memmove, and memset calls to host implementations.

#### Debug Output
```cpp
#ifdef _WIN32
GUEST_FUNCTION_HOOK(sub_82BD4CA8, OutputDebugStringA);
#else
GUEST_FUNCTION_STUB(sub_82BD4CA8);
#endif
```
**Purpose**: Debug output function hook. Uses OutputDebugStringA on Windows, stubbed on other platforms.

#### Timing Functions
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4AC8, QueryPerformanceCounterImpl);
GUEST_FUNCTION_HOOK(sub_831CD040, QueryPerformanceFrequencyImpl);
GUEST_FUNCTION_HOOK(sub_831CDAD0, GetTickCountImpl);
```
**Purpose**: High-resolution timing functions for performance measurement and frame timing.

#### Memory Status
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4BC0, GlobalMemoryStatusImpl);
```
**Purpose**: Memory status reporting function that provides system memory information to the game.

---

## video.cpp

### Graphics and Rendering Functions

#### `sub_824ECA00` (Present/Swap Chain)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824ECA00);
//PPC_FUNC(sub_824ECA00)
//{
//    // Guard against thread ownership changes when between command lists.
//    g_readyForCommands.wait(false);
//    g_presentThreadId = std::this_thread::get_id();
//    __imp__sub_824ECA00(ctx, base);
//}
```
**Purpose**: Present/swap chain management function that ensures proper thread ownership for rendering commands.

#### `sub_82E9F048` (Render Target Resolution Fix)
```cpp
//PPC_FUNC(sub_82E9F048)
//{
//    PPC_STORE_U8(ctx.r4.u32 + 20, 1);
//    PPC_STORE_U32(ctx.r4.u32 + 44, PPC_LOAD_U32(ctx.r4.u32 + 8)); // Width
//    PPC_STORE_U32(ctx.r4.u32 + 48, PPC_LOAD_U32(ctx.r4.u32 + 12)); // Height
//}
```
**Purpose**: Fixes issues with render targets above 1024x1024 resolution by setting appropriate flags and copying dimensions.

#### Shader Management Functions

#### `sub_82E33330` (Vertex Shader Creation)
```cpp
PPC_FUNC_IMPL(__imp__sub_82E33330);
PPC_FUNC(sub_82E33330)
{
    auto vertexShaderCode = reinterpret_cast<Hedgehog::Mirage::CVertexShaderCodeData*>(g_memory.Translate(ctx.r4.u32));
    __imp__sub_82E33330(ctx, base);
    reinterpret_cast<GuestShader*>(vertexShaderCode->m_pD3DVertexShader.get())->name = vertexShaderCode->m_TypeAndName.c_str() + 3;
}
```
**Purpose**: Vertex shader creation hook that extracts shader names from the Hedgehog engine shader code data structure.

#### `sub_82E328D8` (Pixel Shader Creation)
```cpp
PPC_FUNC_IMPL(__imp__sub_82E328D8);
PPC_FUNC(sub_82E328D8)
{
    auto pixelShaderCode = reinterpret_cast<Hedgehog::Mirage::CPixelShaderCodeData*>(g_memory.Translate(ctx.r4.u32));
    __imp__sub_82E328D8(ctx, base);
    reinterpret_cast<GuestShader*>(pixelShaderCode->m_pD3DPixelShader.get())->name = pixelShaderCode->m_TypeAndName.c_str() + 2;
}
```
**Purpose**: Pixel shader creation hook that extracts shader names from the Hedgehog engine shader code data structure.

### Device and Resource Management

#### `sub_8258C8A0` (Device Creation)
```cpp
//PPC_FUNC_IMPL(__imp__sub_8258C8A0);
//PPC_FUNC(sub_8258C8A0)
```
**Purpose**: Direct3D device creation function hook.

#### `sub_8258CAE0` (Device Reset)
```cpp
//PPC_FUNC_IMPL(__imp__sub_8258CAE0);
//PPC_FUNC(sub_8258CAE0)
```
**Purpose**: Direct3D device reset function hook.

### Texture and Surface Management

#### `sub_824EB5B0` (Texture Lock)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824EB5B0);
//PPC_FUNC(sub_824EB5B0)
```
**Purpose**: Texture locking function for CPU access to texture data.

#### `sub_824EB290` (Texture Unlock)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824EB290);
//PPC_FUNC(sub_824EB290)
```
**Purpose**: Texture unlocking function after CPU access is complete.

#### `sub_824860E0` (Surface Lock)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824860E0);
//PPC_FUNC(sub_824860E0)
```
**Purpose**: Surface locking function for CPU access to surface data.

#### `sub_824831D0` (Surface Unlock)
```cpp
//PPC_FUNC_IMPL(__imp__sub_824831D0);
//PPC_FUNC(sub_824831D0)
```
**Purpose**: Surface unlocking function after CPU access is complete.

### Rendering Pipeline Functions

#### `sub_825369A0` (Draw Primitive)
```cpp
//PPC_FUNC_IMPL(__imp__sub_825369A0);
//PPC_FUNC(sub_825369A0)
```
**Purpose**: Basic primitive drawing function.

#### `sub_82E2EFB0` (Indexed Drawing)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E2EFB0);
//PPC_FUNC(sub_82E2EFB0)
```
**Purpose**: Indexed primitive drawing function.

#### `sub_82E243D8` (Draw Primitive UP)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E243D8);
//PPC_FUNC(sub_82E243D8)
```
**Purpose**: User pointer primitive drawing function.

#### `sub_82E87598` (Clear)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E87598);
//PPC_FUNC(sub_82E87598)
```
**Purpose**: Render target and depth/stencil clearing function.

### State Management Functions

#### `sub_825E4300` (Render State)
```cpp
//PPC_FUNC_IMPL(__imp__sub_825E4300);
//PPC_FUNC(sub_825E4300)
```
**Purpose**: Render state setting function.

#### `sub_825E2F78` (Texture State)
```cpp
//PPC_FUNC_IMPL(__imp__sub_825E2F78);
//PPC_FUNC(sub_825E2F78)
```
**Purpose**: Texture state setting function.

#### `sub_82E44AF8` (Sampler State)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E44AF8);
//PPC_FUNC(sub_82E44AF8)
```
**Purpose**: Sampler state setting function.

### Viewport and Scissor Functions

#### `sub_82E250D0` (Viewport)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E250D0);
//PPC_FUNC(sub_82E250D0)
```
**Purpose**: Viewport setting function.

#### `sub_82E3AFC8` (Scissor Rect)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E3AFC8);
//PPC_FUNC(sub_82E3AFC8)
```
**Purpose**: Scissor rectangle setting function.

### Shader Constant Functions

#### `sub_82E3B1C0` (Vertex Shader Constants)
```cpp
//PPC_FUNC_IMPL(__imp__sub_82E3B1C0);
//PPC_FUNC(sub_82E3B1C0)
```
**Purpose**: Vertex shader constant setting function.

---

## Summary

### Function Categories by Purpose

1. **System Initialization**: `sub_824EB490` (App constructor)
2. **Main Loop**: `sub_822C1130` (App update)
3. **Memory Management**: memcpy, memset, memmove hooks
4. **Timing**: Performance counter and tick count functions
5. **Debug Output**: Debug string output functions
6. **Graphics Device**: Device creation, reset, and management
7. **Resource Management**: Texture and surface creation/locking
8. **Rendering**: Draw calls, clear operations, state management
9. **Shader Management**: Shader creation and constant setting
10. **Pipeline State**: Viewport, scissor, render states

### Implementation Patterns

- **Active Hooks**: Fully implemented functions that add custom behavior
- **Pass-through Hooks**: Functions that call original implementation after custom logic
- **Stub Hooks**: Empty implementations for unimplemented functions
- **Conditional Hooks**: Platform-specific implementations (Windows vs others)

### Key Architectural Concepts

1. **Big-Endian to Little-Endian Translation**: All memory operations handle Xbox 360's big-endian format
2. **Thread Safety**: Proper synchronization for multi-threaded rendering
3. **Resource Lifetime Management**: Proper cleanup and reference counting
4. **Performance Optimization**: Minimized overhead in hot rendering paths
5. **Debug Support**: Extensive debugging and profiling capabilities

This comprehensive hooking system allows Framework Recomp to intercept and modify any Xbox 360 function call, enabling full compatibility and performance optimization on modern hardware.
