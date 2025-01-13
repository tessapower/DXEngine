# Completion Source files
file(GLOB SOURCES *.c *.cpp)

list(APPEND
    SOURCES
    "CMakeLists.txt"
)

set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/renderer/renderer.h"
)

add_library(renderer STATIC
    ${SOURCES}
    ${HEADERS}
)

# Set the runtime path to find the shaders
target_compile_definitions(renderer PRIVATE SHADER_SRC_DIR="${CMAKE_SOURCE_DIR}/shaders")
target_compile_definitions(renderer PRIVATE COMPILED_SHADER_DIR="${CMAKE_BINARY_DIR}/shaders/compiled_shaders")

target_link_libraries(renderer PRIVATE imgui::imgui)
target_link_libraries(renderer PRIVATE d3d11)

# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(renderer PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(renderer PRIVATE ../include/renderer)
