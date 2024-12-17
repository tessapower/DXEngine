# Completion Source files
file(GLOB SOURCES *.c *.cpp)

list(APPEND
    SOURCES
    "CMakeLists.txt"
)

set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/app.h"
    "${PROJECT_SOURCE_DIR}/include/engine_exception.h"
    "${PROJECT_SOURCE_DIR}/include/exception_macros.h"
    "${PROJECT_SOURCE_DIR}/include/gui.h"
    "${PROJECT_SOURCE_DIR}/include/message_map.h"
    "${PROJECT_SOURCE_DIR}/include/renderer.h"
    "${PROJECT_SOURCE_DIR}/include/resource.h"
    "${PROJECT_SOURCE_DIR}/include/scene.h"
    "${PROJECT_SOURCE_DIR}/include/stdafx.h"
)

add_executable(DXEngine ${SOURCES} ${HEADERS})

# Ensure shaders are built before the application
add_dependencies(DXEngine Shaders)

# Set the runtime path to find the shaders
target_compile_definitions(DXEngine PRIVATE SHADER_DIR="${CMAKE_BINARY_DIR}/shaders/compiled_shaders")

target_link_libraries(DXEngine PRIVATE imgui::imgui)
target_link_libraries(DXEngine PRIVATE d3d11)
