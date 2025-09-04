# GUEST_FUNCTION Documentation

This document catalogs all GUEST_FUNCTION implementations across the Legacy Recomp codebase, explaining their purpose and functionality.

## Overview

GUEST_FUNCTION macros are used to hook Xbox 360 system calls and redirect them to host implementations. These macros create the bridge between the guest PowerPC code and the host system.

### Macro Definitions

- `GUEST_FUNCTION_HOOK(subroutine, function)` - Hooks a PPC function to a host function implementation
- `GUEST_FUNCTION_STUB(subroutine)` - Creates an empty stub implementation
- `GUEST_FUNCTION_STUB_P(subroutine)` - Creates a stub with debug output

---

## main.cpp

### String Formatting Functions (GUEST_FUNCTION_STUB_P)

#### `vsprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp__vsprintf);
```
**Purpose**: Variable argument sprintf function stub with debug output.

#### `_vsnprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp___vsnprintf);
```
**Purpose**: Variable argument snprintf function stub with debug output.

#### `sprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp__sprintf);
```
**Purpose**: Standard sprintf function stub with debug output.

#### `_snprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp___snprintf);
```
**Purpose**: Standard snprintf function stub with debug output.

#### `_snwprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp___snwprintf);
```
**Purpose**: Wide character snprintf function stub with debug output.

#### `vswprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp__vswprintf);
```
**Purpose**: Variable argument wide character sprintf function stub with debug output.

#### `_vscwprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp___vscwprintf);
```
**Purpose**: Variable argument wide character count sprintf function stub with debug output.

#### `swprintf`
```cpp
GUEST_FUNCTION_STUB_P(__imp__swprintf);
```
**Purpose**: Wide character sprintf function stub with debug output.

---

## misc_impl.cpp

### Memory Functions (GUEST_FUNCTION_HOOK)

#### memcpy Implementations
```cpp
GUEST_FUNCTION_HOOK(sub_831B0ED0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CCB98, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEAE0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CEE04, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF2D0, memcpy);
GUEST_FUNCTION_HOOK(sub_831CF660, memcpy);
GUEST_FUNCTION_HOOK(sub_831B1358, memcpy);
```
**Purpose**: Multiple memcpy function hooks that redirect PPC memcpy calls to host memcpy implementations.

#### `memmove`
```cpp
GUEST_FUNCTION_HOOK(sub_831B5E00, memmove);
```
**Purpose**: Memory move function hook that handles overlapping memory regions.

#### memset Implementations
```cpp
GUEST_FUNCTION_HOOK(sub_831B0BA0, memset);
GUEST_FUNCTION_HOOK(sub_831CCAA0, memset);
```
**Purpose**: Memory set function hooks that redirect PPC memset calls to host memset implementations.

### Debug Output (GUEST_FUNCTION_HOOK/GUEST_FUNCTION_STUB)

#### `OutputDebugStringA`
```cpp
#ifdef _WIN32
GUEST_FUNCTION_HOOK(sub_82BD4CA8, OutputDebugStringA);
#else
GUEST_FUNCTION_STUB(sub_82BD4CA8);
#endif
```
**Purpose**: Debug output function that uses Windows OutputDebugStringA on Windows or stub on other platforms.

### Timing Functions (GUEST_FUNCTION_HOOK)

#### `QueryPerformanceCounterImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4AC8, QueryPerformanceCounterImpl);
```
**Purpose**: High-resolution performance counter for timing measurements.

#### `QueryPerformanceFrequencyImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_831CD040, QueryPerformanceFrequencyImpl);
```
**Purpose**: Gets the frequency of the performance counter.

#### `GetTickCountImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_831CDAD0, GetTickCountImpl);
```
**Purpose**: Gets the number of milliseconds since system startup.

### Memory Status (GUEST_FUNCTION_HOOK)

#### `GlobalMemoryStatusImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4BC0, GlobalMemoryStatusImpl);
```
**Purpose**: Provides system memory status information to the game.

---

## heap.cpp

### Heap Management Functions

#### Heap Creation/Destruction (GUEST_FUNCTION_STUB)

#### `HeapCreate`
```cpp
GUEST_FUNCTION_STUB(sub_82BD7788); // HeapCreate
```
**Purpose**: Windows heap creation function stub.

#### `HeapDestroy`
```cpp
GUEST_FUNCTION_STUB(sub_82BD9250); // HeapDestroy
```
**Purpose**: Windows heap destruction function stub.

### Heap Operations (GUEST_FUNCTION_HOOK)

#### `RtlAllocateHeap`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD7D30, RtlAllocateHeap);
```
**Purpose**: Windows heap allocation function that uses the custom heap implementation.

#### `RtlFreeHeap`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD8600, RtlFreeHeap);
```
**Purpose**: Windows heap deallocation function.

#### `RtlReAllocateHeap`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD88F0, RtlReAllocateHeap);
```
**Purpose**: Windows heap reallocation function.

#### `RtlSizeHeap`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD6FD0, RtlSizeHeap);
```
**Purpose**: Gets the size of a heap allocation.

### Xbox Memory Functions (GUEST_FUNCTION_HOOK)

#### `XAllocMem`
```cpp
GUEST_FUNCTION_HOOK(sub_831CC9C8, XAllocMem);
```
**Purpose**: Xbox-specific memory allocation function with physical memory support.

#### `XFreeMem`
```cpp
GUEST_FUNCTION_HOOK(sub_831CCA60, XFreeMem);
```
**Purpose**: Xbox-specific memory deallocation function.

---

## guest_thread.cpp

### Thread Management Functions

#### `SetThreadNameImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_82DFA2E8, SetThreadNameImpl);
```
**Purpose**: Sets the name of a thread for debugging purposes.

#### `GetThreadPriorityImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD57A8, GetThreadPriorityImpl);
```
**Purpose**: Gets the priority of a thread.

#### `SetThreadIdealProcessorImpl`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD5910, SetThreadIdealProcessorImpl);
```
**Purpose**: Sets the ideal processor for a thread.

#### TEB Update Function (GUEST_FUNCTION_STUB)

#### `sub_82BD58F8`
```cpp
GUEST_FUNCTION_STUB(sub_82BD58F8); // Some function that updates the TEB, don't really care since the field is not set
```
**Purpose**: Thread Environment Block update function stub (not needed for compatibility).

---

## file_system.cpp

### File System Operations (GUEST_FUNCTION_HOOK)

#### `XCreateFileA`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4668, XCreateFileA);
GUEST_FUNCTION_HOOK(sub_831CE3F8, XCreateFileA);
```
**Purpose**: File creation function that maps Xbox file paths to host filesystem.

#### `XGetFileSizeA`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4600, XGetFileSizeA);
```
**Purpose**: Gets the size of a file.

#### `XGetFileSizeExA`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD5608, XGetFileSizeExA);
```
**Purpose**: Extended file size function with 64-bit support.

#### `XReadFile`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4478, XReadFile);
```
**Purpose**: File reading function with overlapped I/O support.

#### `XSetFilePointer`
```cpp
GUEST_FUNCTION_HOOK(sub_831CD3E8, XSetFilePointer);
```
**Purpose**: Sets the file pointer position.

#### `XSetFilePointerEx`
```cpp
GUEST_FUNCTION_HOOK(sub_831CE888, XSetFilePointerEx);
```
**Purpose**: Extended file pointer setting with 64-bit support.

#### `XFindFirstFileA`
```cpp
GUEST_FUNCTION_HOOK(sub_831CDC58, XFindFirstFileA);
```
**Purpose**: Finds the first file matching a pattern.

#### `XFindNextFileA`
```cpp
GUEST_FUNCTION_HOOK(sub_831CDC00, XFindNextFileA);
```
**Purpose**: Finds the next file in a search operation.

#### `XReadFileEx`
```cpp
GUEST_FUNCTION_HOOK(sub_831CDF40, XReadFileEx);
```
**Purpose**: Extended file reading with completion routines.

#### `XGetFileAttributesA`
```cpp
GUEST_FUNCTION_HOOK(sub_831CD6E8, XGetFileAttributesA);
```
**Purpose**: Gets file attributes (directory, normal file, etc.).

#### `XWriteFile`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD4860, XWriteFile);
```
**Purpose**: File writing function with overlapped I/O support.

---

## function.h

### Core Function Hooking System

#### `GUEST_FUNCTION_HOOK` Macro
```cpp
#define GUEST_FUNCTION_HOOK(subroutine, function) \
    PPC_FUNC(subroutine) { HostToGuestFunction<function>(ctx, base); }
```
**Purpose**: Core macro that creates a PPC function hook that redirects to a host function implementation.

#### `GUEST_FUNCTION_STUB` Macro
```cpp
#define GUEST_FUNCTION_STUB(subroutine) \
    PPC_FUNC(subroutine) { }
```
**Purpose**: Macro that creates an empty stub implementation for PPC functions.

#### `GUEST_FUNCTION_STUB_P` Macro
```cpp
#define GUEST_FUNCTION_STUB_P(subroutine) \
    PPC_FUNC(subroutine) { printf(#subroutine " is Stubbed\n"); }
```
**Purpose**: Macro that creates a stub with debug output to indicate when a function is called.

---

## imports.cpp

### Xbox Application Management (XAM) Functions

#### Video and Display Functions (GUEST_FUNCTION_HOOK)

#### `XGetVideoMode`
```cpp
GUEST_FUNCTION_HOOK(__imp__XGetVideoMode, VdQueryVideoMode); // XGetVideoMode
```
**Purpose**: Gets the current video mode settings.

#### `XNotifyGetNext`
```cpp
GUEST_FUNCTION_HOOK(__imp__XNotifyGetNext, XNotifyGetNext);
```
**Purpose**: Gets the next notification from the Xbox notification system.

#### `XGetGameRegion`
```cpp
GUEST_FUNCTION_HOOK(__imp__XGetGameRegion, XGetGameRegion);
```
**Purpose**: Gets the game region setting.

#### `XGetLanguage`
```cpp
GUEST_FUNCTION_HOOK(__imp__XGetLanguage, XGetLanguage);
```
**Purpose**: Gets the current language setting.

#### `XGetAVPack`
```cpp
GUEST_FUNCTION_HOOK(__imp__XGetAVPack, XGetAVPack);
```
**Purpose**: Gets the A/V pack configuration.

### User and Content Management (GUEST_FUNCTION_HOOK)

#### `XamUserGetSigninState`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamUserGetSigninState, XamUserGetSigninState);
```
**Purpose**: Gets the sign-in state of a user.

#### `XamGetSystemVersion`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamGetSystemVersion, XamGetSystemVersion);
```
**Purpose**: Gets the system version.

#### `XamUserGetSigninInfo`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamUserGetSigninInfo, XamUserGetSigninInfo);
```
**Purpose**: Gets detailed sign-in information for a user.

#### `XamUserReadProfileSettings`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamUserReadProfileSettings, XamUserReadProfileSettings);
```
**Purpose**: Reads user profile settings.

### Content Operations (GUEST_FUNCTION_HOOK)

#### `XamContentCreateEx`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamContentCreateEx, XamContentCreateEx);
```
**Purpose**: Creates or opens content containers.

#### `XamContentDelete`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamContentDelete, XamContentDelete);
```
**Purpose**: Deletes content from the system.

#### `XamContentClose`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamContentClose, XamContentClose);
```
**Purpose**: Closes a content container.

#### `XamContentGetCreator`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamContentGetCreator, XamContentGetCreator);
```
**Purpose**: Gets the creator information for content.

#### `XamEnumerate`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamEnumerate, XamEnumerate);
```
**Purpose**: Enumerates items in a collection.

### UI Functions (GUEST_FUNCTION_HOOK)

#### `XamShowSigninUI`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamShowSigninUI, XamShowSigninUI);
```
**Purpose**: Shows the user sign-in interface.

#### `XamShowDeviceSelectorUI`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamShowDeviceSelectorUI, XamShowDeviceSelectorUI);
```
**Purpose**: Shows the device selector interface.

#### `XamShowMessageBoxUI`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamShowMessageBoxUI, XamShowMessageBoxUI);
```
**Purpose**: Shows a message box to the user.

#### `XamShowDirtyDiscErrorUI`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamShowDirtyDiscErrorUI, XamShowDirtyDiscErrorUI);
```
**Purpose**: Shows the dirty disc error interface.

### System Functions (GUEST_FUNCTION_HOOK)

#### `XamLoaderTerminateTitle`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamLoaderTerminateTitle, XamLoaderTerminateTitle);
```
**Purpose**: Terminates the current title.

#### `XamGetExecutionId`
```cpp
GUEST_FUNCTION_HOOK(__imp__XamGetExecutionId, XamGetExecutionId);
```
**Purpose**: Gets the execution ID of the current process.

### Kernel Functions (GUEST_FUNCTION_HOOK)

#### `NtOpenFile`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtOpenFile, NtOpenFile);
```
**Purpose**: Opens a file using NT kernel functions.

#### `NtCreateFile`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtCreateFile, NtCreateFile);
```
**Purpose**: Creates or opens a file.

#### `NtClose`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtClose, NtClose);
```
**Purpose**: Closes a handle.

#### `NtSetInformationFile`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtSetInformationFile, NtSetInformationFile);
```
**Purpose**: Sets file information.

#### `NtWaitForSingleObjectEx`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtWaitForSingleObjectEx, NtWaitForSingleObjectEx);
```
**Purpose**: Waits for a single object with extended options.

#### `NtWriteFile`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtWriteFile, NtWriteFile);
```
**Purpose**: Writes data to a file.

#### `NtCreateEvent`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtCreateEvent, NtCreateEvent);
```
**Purpose**: Creates an event object.

#### `NtQueryVirtualMemory`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtQueryVirtualMemory, NtQueryVirtualMemory);
```
**Purpose**: Queries virtual memory information.

#### `NtAllocateVirtualMemory`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtAllocateVirtualMemory, NtAllocateVirtualMemory);
```
**Purpose**: Allocates virtual memory.

#### `NtFreeVirtualMemory`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtFreeVirtualMemory, NtFreeVirtualMemory);
```
**Purpose**: Frees virtual memory.

### Thread Management (GUEST_FUNCTION_HOOK)

#### `KeSetBasePriorityThread`
```cpp
GUEST_FUNCTION_HOOK(__imp__KeSetBasePriorityThread, KeSetBasePriorityThread);
```
**Purpose**: Sets the base priority of a thread.

#### `KeQueryBasePriorityThread`
```cpp
GUEST_FUNCTION_HOOK(__imp__KeQueryBasePriorityThread, KeQueryBasePriorityThread);
```
**Purpose**: Queries the base priority of a thread.

#### `NtSuspendThread`
```cpp
GUEST_FUNCTION_HOOK(__imp__NtSuspendThread, NtSuspendThread);
```
**Purpose**: Suspends a thread.

#### `KeSetAffinityThread`
```cpp
GUEST_FUNCTION_HOOK(__imp__KeSetAffinityThread, KeSetAffinityThread);
```
**Purpose**: Sets the processor affinity for a thread.

### Synchronization Functions (GUEST_FUNCTION_HOOK)

#### `RtlLeaveCriticalSection`
```cpp
GUEST_FUNCTION_HOOK(__imp__RtlLeaveCriticalSection, RtlLeaveCriticalSection);
```
**Purpose**: Leaves a critical section.

#### `RtlEnterCriticalSection`
```cpp
GUEST_FUNCTION_HOOK(__imp__RtlEnterCriticalSection, RtlEnterCriticalSection);
```
**Purpose**: Enters a critical section.

#### `KfReleaseSpinLock`
```cpp
GUEST_FUNCTION_HOOK(__imp__KfReleaseSpinLock, KfReleaseSpinLock);
```
**Purpose**: Releases a spin lock.

#### `KfAcquireSpinLock`
```cpp
GUEST_FUNCTION_HOOK(__imp__KfAcquireSpinLock, KfAcquireSpinLock);
```
**Purpose**: Acquires a spin lock.

#### `KeWaitForSingleObject`
```cpp
GUEST_FUNCTION_HOOK(__imp__KeWaitForSingleObject, KeWaitForSingleObject);
```
**Purpose**: Waits for a kernel object.

### Memory Management (GUEST_FUNCTION_HOOK)

#### `MmFreePhysicalMemory`
```cpp
GUEST_FUNCTION_HOOK(__imp__MmFreePhysicalMemory, MmFreePhysicalMemory);
```
**Purpose**: Frees physical memory.

#### `MmGetPhysicalAddress`
```cpp
GUEST_FUNCTION_HOOK(__imp__MmGetPhysicalAddress, MmGetPhysicalAddress);
```
**Purpose**: Gets the physical address of virtual memory.

### Video Driver Functions (GUEST_FUNCTION_HOOK)

#### `VdPersistDisplay`
```cpp
GUEST_FUNCTION_HOOK(__imp__VdPersistDisplay, VdPersistDisplay);
```
**Purpose**: Persists display settings.

#### `VdSwap`
```cpp
GUEST_FUNCTION_HOOK(__imp__VdSwap, VdSwap);
```
**Purpose**: Swaps video buffers.

#### `VdQueryVideoMode`
```cpp
GUEST_FUNCTION_HOOK(__imp__VdQueryVideoMode, VdQueryVideoMode);
```
**Purpose**: Queries the current video mode.

#### `VdInitializeEngines`
```cpp
GUEST_FUNCTION_HOOK(__imp__VdInitializeEngines, VdInitializeEngines);
```
**Purpose**: Initializes video engines.

### Stub Functions (GUEST_FUNCTION_STUB_P)

The following functions are stubbed with debug output when called:

#### Content Management Stubs
- `XamLoaderGetLaunchData`, `XamLoaderGetLaunchDataSize`, `XamLoaderSetLaunchData`
- `XamReadTileToTexture`, `XamWriteGamerTile`
- `XamContentFlush`, `XamContentInstall`, `XamContentResolve`

#### User Interface Stubs
- `XamShowFriendsUI`, `XamShowAchievementsUI`, `XamShowGamerCardUIForXUID`
- `XamShowKeyboardUI`, `XamShowMessagesUI`, `XamShowPlayersUI`
- `XamShowMarketplaceUI`, `XamShowMessageComposeUI`

#### Network Stubs
- `NetDll_WSAStartup`, `NetDll_WSACleanup`, `NetDll_socket`
- `NetDll_XNetConnect`, `NetDll_XNetGetTitleXnAddr`

#### System Stubs
- `KeBugCheck`, `KeBugCheckEx`, `HalReturnToFirmware`
- `XexUnloadImage`, `XexLoadImage`

---

## video.cpp

### Graphics Device Management (GUEST_FUNCTION_HOOK)

#### `CreateDevice`
```cpp
GUEST_FUNCTION_HOOK(sub_82BD99B0, CreateDevice);
```
**Purpose**: Creates a Direct3D device.

#### `DestructResource`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6230, DestructResource);
```
**Purpose**: Destroys a graphics resource.

### Resource Management (GUEST_FUNCTION_HOOK)

#### `LockTextureRect`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE9300, LockTextureRect);
```
**Purpose**: Locks a texture for CPU access.

#### `UnlockTextureRect`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE7780, UnlockTextureRect);
```
**Purpose**: Unlocks a texture after CPU access.

#### `LockVertexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6B98, LockVertexBuffer);
```
**Purpose**: Locks a vertex buffer for CPU access.

#### `UnlockVertexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6BE8, UnlockVertexBuffer);
```
**Purpose**: Unlocks a vertex buffer after CPU access.

#### `LockIndexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6CA8, LockIndexBuffer);
```
**Purpose**: Locks an index buffer for CPU access.

#### `UnlockIndexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6CF0, UnlockIndexBuffer);
```
**Purpose**: Unlocks an index buffer after CPU access.

### Resource Creation (GUEST_FUNCTION_HOOK)

#### `CreateTexture`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE9498, CreateTexture);
```
**Purpose**: Creates a texture resource.

#### `CreateVertexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6AD0, CreateVertexBuffer);
```
**Purpose**: Creates a vertex buffer.

#### `CreateIndexBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE6BF8, CreateIndexBuffer);
```
**Purpose**: Creates an index buffer.

#### `CreateSurface`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE95B8, CreateSurface);
```
**Purpose**: Creates a render surface.

### Rendering Operations (GUEST_FUNCTION_HOOK)

#### `DrawPrimitive`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE5900, DrawPrimitive);
```
**Purpose**: Draws primitives from vertex data.

#### `DrawIndexedPrimitive`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE5CF0, DrawIndexedPrimitive);
```
**Purpose**: Draws indexed primitives.

#### `DrawPrimitiveUP`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE52F8, DrawPrimitiveUP);
```
**Purpose**: Draws primitives from user pointer data.

### State Management (GUEST_FUNCTION_HOOK)

#### `SetRenderTarget`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDD9F0, SetRenderTarget);
```
**Purpose**: Sets the render target surface.

#### `SetDepthStencilSurface`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDDD38, SetDepthStencilSurface);
```
**Purpose**: Sets the depth/stencil surface.

#### `SetViewport`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDD8C0, SetViewport);
```
**Purpose**: Sets the viewport rectangle.

#### `SetTexture`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE9818, SetTexture);
```
**Purpose**: Sets a texture for rendering.

#### `SetScissorRect`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDCFB0, SetScissorRect);
```
**Purpose**: Sets the scissor rectangle.

### Shader Management (GUEST_FUNCTION_HOOK)

#### `CreateVertexShader`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE1A80, CreateVertexShader);
```
**Purpose**: Creates a vertex shader.

#### `SetVertexShader`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE0110, SetVertexShader);
```
**Purpose**: Sets the active vertex shader.

#### `CreatePixelShader`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE1990, CreatePixelShader);
```
**Purpose**: Creates a pixel shader.

#### `SetPixelShader`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDFE58, SetPixelShader);
```
**Purpose**: Sets the active pixel shader.

### Vertex Declaration (GUEST_FUNCTION_HOOK)

#### `CreateVertexDeclaration`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE0428, CreateVertexDeclaration);
```
**Purpose**: Creates a vertex declaration.

#### `SetVertexDeclaration`
```cpp
GUEST_FUNCTION_HOOK(sub_82BE02E0, SetVertexDeclaration);
```
**Purpose**: Sets the active vertex declaration.

### Stream Management (GUEST_FUNCTION_HOOK)

#### `SetStreamSource`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDD0F8, SetStreamSource);
```
**Purpose**: Sets a vertex stream source.

#### `SetIndices`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDD218, SetIndices);
```
**Purpose**: Sets the index buffer.

### Present and Display (GUEST_FUNCTION_HOOK)

#### `Present`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDA8C0, Video::Present);
```
**Purpose**: Presents the rendered frame to the display.

#### `GetBackBuffer`
```cpp
GUEST_FUNCTION_HOOK(sub_82BDD330, GetBackBuffer);
```
**Purpose**: Gets the back buffer surface.

### Utility Functions (GUEST_FUNCTION_HOOK)

#### `StretchRect`
```cpp
GUEST_FUNCTION_HOOK(sub_82BF6400, StretchRect);
```
**Purpose**: Copies and stretches a rectangle between surfaces.

#### `Clear`
```cpp
GUEST_FUNCTION_HOOK(sub_82BFE4C8, Clear);
```
**Purpose**: Clears render targets and depth/stencil buffers.

### Stub Functions (GUEST_FUNCTION_STUB)

#### `sub_82BAAD38`
```cpp
GUEST_FUNCTION_STUB(sub_82BAAD38);
```
**Purpose**: Graphics function stub.

#### `sub_822C15D8`
```cpp
GUEST_FUNCTION_STUB(sub_822C15D8);
```
**Purpose**: Graphics function stub.

#### `sub_822C1810`
```cpp
GUEST_FUNCTION_STUB(sub_822C1810);
```
**Purpose**: Graphics function stub.

#### `sub_82BD97A8`
```cpp
GUEST_FUNCTION_STUB(sub_82BD97A8);
```
**Purpose**: Graphics function stub.

#### `sub_82BD97E8`
```cpp
GUEST_FUNCTION_STUB(sub_82BD97E8);
```
**Purpose**: Graphics function stub.

#### `sub_82BDD370`
```cpp
GUEST_FUNCTION_STUB(sub_82BDD370); // SetGammaRamp
```
**Purpose**: Set gamma ramp function stub.

#### `sub_82BE05B8`
```cpp
GUEST_FUNCTION_STUB(sub_82BE05B8);
```
**Purpose**: Graphics function stub.

---

## Summary

### Function Categories by Purpose

1. **Memory Management**: memcpy, memset, memmove, heap functions, Xbox memory allocation
2. **File System**: File creation, reading, writing, directory operations
3. **Threading**: Thread creation, priority, affinity, synchronization
4. **Graphics**: Device creation, resource management, rendering operations, shaders
5. **System Services**: User management, content operations, notifications
6. **Kernel Services**: NT kernel functions, virtual memory, synchronization
7. **UI Management**: Sign-in, device selection, message boxes, marketplace
8. **Timing**: Performance counters, tick counts, system time
9. **Debug**: Output functions, breakpoints, exception handling
10. **Network**: Socket operations, XNet functions (mostly stubbed)

### Implementation Patterns

- **Active Hooks**: Fully implemented functions that provide real functionality
- **Pass-through Hooks**: Functions that call host implementations directly
- **Stub Hooks**: Empty implementations for unimplemented functions
- **Platform-specific**: Windows-specific implementations with fallbacks
- **Debug Stubs**: Stubs with logging for development purposes

### Key Architectural Concepts

1. **Cross-platform Compatibility**: Windows-specific functions with Unix fallbacks
2. **Resource Management**: Proper cleanup and reference counting for graphics resources
3. **Thread Safety**: Synchronization primitives for multi-threaded operations
4. **Memory Translation**: Big-endian to little-endian conversion for Xbox 360 compatibility
5. **Error Handling**: Proper error codes and status reporting
6. **Performance Optimization**: Efficient implementations for hot rendering paths

This comprehensive hooking system provides a complete bridge between Xbox 360 system calls and modern host system implementations, enabling full compatibility while maintaining performance.
