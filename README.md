# DXEngine

A modern C++23 DirectX 11 rendering engine built with a focus on clean architecture, performance, and extensibility. DXEngine provides a robust foundation for 3D graphics applications on Windows, featuring a modular bindable system, comprehensive error handling, and an immediate mode GUI.

## Features

### Core Engine
- **Modern C++23**: Leverages the latest C++ features for clean, efficient code
- **DirectX 11 Rendering**: Hardware-accelerated 3D graphics with full D3D11 pipeline support
- **ImGui Integration**: Immediate mode GUI for debugging and tool development
- **Modular Architecture**: Component-based system with clear separation of concerns
- **Exception Safety**: Comprehensive error handling with custom exception hierarchy

### Rendering System
- **Bindable Architecture**: Flexible, type-safe resource binding system
- **Drawable Objects**: Base classes for renderable geometry with transform support
- **HLSL Shader Support**: Vertex and pixel shader compilation and management
- **Buffer Management**: Vertex, index, and constant buffer abstractions
- **Device State Management**: Automatic handling of device loss and recovery

### Development Features
- **DXGI Debug Integration**: Detailed DirectX debug information and error reporting
- **Step Timer**: High-resolution timing for frame rate control and animation
- **Resource Management**: RAII-compliant resource handling with smart pointers
- **Cross-Compiler Support**: Works with MSVC, GCC, and Clang

## Architecture

DXEngine is built around several key architectural patterns:

### Bindable System
The core of the rendering system is the bindable architecture, where render states, shaders, and resources implement a common `bindable` interface. This allows for:
- Type-safe resource binding
- Easy state management
- Flexible render pipeline configuration
- Minimal API calls through intelligent state caching

### Drawable System
3D objects inherit from the `drawable` base class, providing:
- Automatic resource binding during rendering
- Transform matrix management
- Update lifecycle for animations
- Static resource sharing for efficiency

### Exception Hierarchy
Custom exception classes provide detailed error information:
- `engine_exception`: Base exception with file/line information
- `hr_exception`: Windows HRESULT error handling
- `renderer::exception`: DirectX-specific errors
- `device_removed_exception`: Graphics device recovery

## Prerequisites

### System Requirements
- **Operating System**: Windows 10/11 (64-bit)
- **Graphics**: DirectX 11 compatible graphics card
- **Compiler**: 
  - MSVC 2022 (recommended)
  - GCC 9.0+ with MinGW
  - Clang 10.0+

### Development Tools
- **CMake**: 3.30 or higher
- **vcpkg**: Package manager for C++ libraries
- **Visual Studio 2022**: Recommended IDE (Community edition sufficient)

## Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/DXEngine.git
cd DXEngine
```

### 2. Setup vcpkg
Ensure vcpkg is installed and the `VCPKG_ROOT` environment variable is set:
```bash
# Example Windows setup
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg integrate install
```

Set the environment variable:
```bash
set VCPKG_ROOT=C:\path\to\vcpkg
```

### 3. Install Dependencies
Dependencies are automatically managed through vcpkg configuration:
```bash
# Dependencies will be installed automatically during CMake configuration
# Current dependencies:
# - imgui with dx11-binding and win32-binding features
```

### 4. Build the Project
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 5. Run the Application
```bash
# From the build directory
./bin/dx_engine.exe
```

## Project Structure

```
DXEngine/
├── include/                 # Header files
│   ├── renderer/           # Rendering system headers
│   │   ├── bindable/      # Bindable resource abstractions
│   │   └── drawable/      # Drawable object system
│   ├── app.h              # Main application class
│   ├── engine_exception.h # Exception hierarchy
│   └── stdafx.h           # Precompiled header
├── src/                    # Source files
│   ├── renderer/          # Rendering system implementation
│   ├── app.cpp            # Application implementation
│   └── main.cpp           # Entry point
├── shaders/               # HLSL shader files
│   ├── vertex_shader.vs.hlsl
│   └── pixel_shader.ps.hlsl
├── CMakeLists.txt         # CMake configuration
├── vcpkg.json            # Dependency configuration
└── README.md             # This file
```

## Core Classes

### `app`
Main application class handling:
- Window creation and management
- Win32 message pump
- Initialization and shutdown
- Frame timing and main loop

### `renderer`
Core rendering system providing:
- D3D11 device and context management
- Swap chain handling
- Render target management
- Drawing operations

### `bindable`
Abstract base for all bindable resources:
- Vertex/Index buffers
- Shaders (vertex, pixel)
- Constant buffers
- Input layouts
- Render states

### `drawable`
Base class for renderable objects:
- Transform matrix management
- Automatic resource binding
- Update lifecycle
- Static resource optimization

## Development

### Adding New Drawable Objects
1. Create a new class inheriting from `drawable_base<YourClass>`
2. Implement required virtual methods:
   - `transform()`: Return current transform matrix
   - `update()`: Update object state per frame
3. Initialize bindable resources in constructor
4. Register with renderer for drawing

### Creating Custom Bindables
1. Inherit from `bindable` base class
2. Implement `bind(renderer& rndr)` method
3. Use protected helper methods to access renderer internals
4. Follow RAII principles for resource management

### Shader Development
1. Add HLSL files to the `shaders/` directory
2. Update `shaders/CMakeLists.txt` to include new shaders
3. Create corresponding bindable wrappers
4. Implement shader compilation and management

## Configuration

### CMake Options
The build system supports various configuration options:

```bash
# Debug build with full debugging information
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify custom vcpkg toolchain
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg.cmake ..
```

### Compiler Flags
- **MSVC**: `/W4 /MP /std:c++latest /utf-8`
- **GCC/Clang**: `-Wall -Wextra -pedantic -std=c++1z`

## Performance Considerations

### Rendering Optimization
- Static bindables are cached and shared between objects
- Minimal state changes through intelligent binding
- Batch rendering for similar objects
- Efficient memory management with smart pointers

### Memory Management
- RAII throughout the codebase
- Smart pointers for automatic cleanup
- COM pointer management with WRL
- Minimal dynamic allocations in hot paths

## Debugging

### Debug Features
- DXGI debug layer integration for detailed D3D error reporting
- Custom exception hierarchy with file/line information
- ImGui integration for runtime debugging
- Comprehensive error handling and recovery

### Common Issues
1. **Device Lost**: Handled automatically with device recovery
2. **Shader Compilation**: Check HLSL syntax and target profile
3. **Resource Binding**: Verify bindable order and compatibility
4. **Performance**: Use PIX or similar tools for GPU profiling

## Contributing

### Code Style
- Follow existing naming conventions (snake_case for variables/functions)
- Use modern C++ idioms and RAII
- Comprehensive documentation for public APIs
- Exception safety guarantees

### Pull Requests
1. Fork the repository
2. Create a feature branch
3. Implement changes with tests
4. Submit pull request with detailed description

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Tessa Power** - [hello@tessapower.xyz](mailto:hello@tessapower.xyz)

## Acknowledgments

- Microsoft DirectX team for excellent documentation
- ImGui developers for the immediate mode GUI framework
- vcpkg team for dependency management
- The C++ community for modern language features

---

*Built with ❤️ and modern C++*
