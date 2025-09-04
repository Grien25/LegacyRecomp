# Framework Recomp

A static recompilation runtime framework for creating Xbox 360 game ports to modern platforms. Basically UnleashedRecomp stripped bare of its game specific functions and only consisting of the SDK content.

## 📋 Prerequisites

- **CMake 3.21+**
- **C++20 compatible compiler**
- **Git**
- **Xbox 360 game executable** (.xex file, .xexp)
- **[pyghidra-mcp](https://github.com/clearbluejar/pyghidra-mcp)** (recommended for function discovery)

### Windows Specific
- Visual Studio 2022 with C++ workload
- Windows SDK 10.0.20348.0 or later

## 🚀 Quick Start

Fork and clone the repo, replace the toml's in the FrameworkLib folder for your own, replace the Xenon/XenosRecomp modules for your own (if you forked that repo, if my versions worked, keep it), and you will have to make fixes to the CMakeLists.txt

## ⚙️ Configuration

### Basic Configuration Structure

You will probably have to change your toml a little bit to work with our system

```toml
[main]
file_path = "../private/default.xex"
out_directory_path = "../ppc"
switch_table_file_path = "./[JUMPTABLES].toml"
```

4. **Update your configuration** with the discovered function addresses.

## 🛠️ Building

### Windows (Visual Studio)
```bash
# Configure
cmake . --preset windows-release

# Build
cmake --build out/build/windows-release --config Release
```

### Linux/macOS
```bash
# Configure
cmake . --preset x64-Clang-Release

# Build
cmake --build out/build/x64-Clang-Release --config Release
```

### Core System Functions

For any new game, all function addresses that were used for Unleashed WILL have to be replaced. A good place to start is Kernel/heap.cpp and move more along the lines. If you need help finding the functions, take a look at the repo's indexed code search, looking for [PPC_FUNC](https://github.com/search?q=repo%3Asonicnext-dev%2FMarathonRecomp+PPC_FUNC&type=code) and [GUEST_FUNCTION](https://github.com/search?q=repo%3Asonicnext-dev%2FMarathonRecomp+GUEST_FUNCTION&type=code).

1. **Memory Management**
   ```cpp
   // In LegacyRecomp/kernel/heap.cpp
   PPC_FUNC(sub_82345678) { /* Your memory allocation logic */ }
   PPC_FUNC(sub_82345690) { /* Your memory deallocation logic */ }
   ```

2. **File System**
   ```cpp
   // In LegacyRecomp/kernel/io/file_system.cpp
   PPC_FUNC(sub_823456A0) { /* Your file open logic */ }
   PPC_FUNC(sub_823456B0) { /* Your file read logic */ }
   ```

3. **Rendering**
   ```cpp
   // In LegacyRecomp/gpu/video.cpp
   PPC_FUNC(sub_823456C0) { /* Your rendering logic */ }
   ```

### Using GUEST_FUNCTION Hooks

For system-level functions:

```cpp
// In LegacyRecomp/misc_impl.cpp
GUEST_FUNCTION_HOOK(sub_82345678, custom_memory_alloc);
GUEST_FUNCTION_HOOK(sub_82345690, custom_file_open);
```

## 🐛 Troubleshooting

### Common Issues

1. **"Cannot find XEX file"**
   - Verify the `file_path` in your TOML config points to the correct XEX location
   - Ensure the XEX file is not corrupted

2. **"Function not found"**
   - Use pyghidra-mcp to verify function addresses
   - Check if your game uses a different SDK version
   - Compare with known working configurations

3. **Build failures**
   - Ensure you have all prerequisites installed
   - Try a clean build: `rm -rf out/ && cmake . --preset x64-Clang-Release`
   - Check CMake version: `cmake --version`

4. **Runtime crashes**
   - Enable debug logging in your config with DebugBreak. Visual Studio exists for a reason and set them pointers
   - Use the function documentation in `PPC_FUNC.md` and `GUEST_FUNCTION.md` for some ai slop info on where the functions are located in this repo
   - Check memory alignment issues

5. **XenonRecomp does not compile the same**
    - Check that your toml has been updated in the header
    - Edit the CMakeLists.txt to make sure that it fits the number of PPC compiled files that you produced earlier.


## 📚 Documentation

- **[PPC_FUNC.md](PPC_FUNC.md)** - Complete list of PPC function implementations
- **[GUEST_FUNCTION.md](GUEST_FUNCTION.md)** - Guest function hook documentation
- **[LegacyRecompLib/config/](LegacyRecompLib/config/)** - Example configurations

## 🤝 Contributing

Submit a pr if you want ion really care. any help is nice ig

### Adding New Game Support

The framework should work for all games, but if you have any changes that could be made to the system, yet again submit dat PR.

## 📄 License

This project is licensed under the same terms as UnleashedRecomp. See LICENSE file for details.

## 🙏 Credits

- **Original UnleashedRecomp team** - For the base runtime framework
- **Xbox 360 development community (Mostly Xenia and triangle)** - For SDK documentation and tools
- **Reverse engineering community** - For function analysis techniques

---

**Note:** This is a development framework. Expect breaking changes and active development.
