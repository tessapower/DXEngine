# Get all renderer source files
file(GLOB RENDERER_SOURCES *.cpp)

# Get all drawable source files
file(GLOB_RECURSE DRAWABLE_SOURCES drawable/*.cpp)

# Combine all sources together in one list
list(APPEND RENDERER_SOURCES ${DRAWABLE_SOURCES} "CMakeLists.txt")

# Create source groups that will appear inside "Source Files"
source_group("Source Files\\drawable" FILES ${DRAWABLE_SOURCES})

# Get all headers in renderer
file(GLOB RENDERER_HEADERS
    "${PROJECT_SOURCE_DIR}/include/renderer/*.h"
)

# Get all headers in bindable
file(GLOB_RECURSE BINDABLE_HEADERS
    "${PROJECT_SOURCE_DIR}/include/renderer/bindable/*.h"
)

# Get all headers in drawable
file(GLOB_RECURSE DRAWABLE_HEADERS
    "${PROJECT_SOURCE_DIR}/include/renderer/drawable/*.h"
)

source_group("Header Files\\bindable" FILES ${BINDABLE_HEADERS})
source_group("Header Files\\drawable" FILES ${DRAWABLE_HEADERS})

# Combine all headers together correctly
list(APPEND RENDERER_HEADERS ${BINDABLE_HEADERS} ${DRAWABLE_HEADERS})

add_library(Renderer STATIC
    ${RENDERER_SOURCES}
    ${RENDERER_HEADERS}
)

set_property(TARGET Renderer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$(SolutionDir)..")

# Set the runtime path to find the shaders
target_compile_definitions(Renderer PRIVATE SHADER_SRC_DIR="${CMAKE_SOURCE_DIR}/shaders")
target_compile_definitions(Renderer PRIVATE COMPILED_SHADER_DIR="${CMAKE_BINARY_DIR}/shaders/compiled_shaders")

target_link_libraries(Renderer PRIVATE imgui::imgui)
target_link_libraries(Renderer PRIVATE d3d11)
target_link_libraries(Renderer PRIVATE d3dcompiler)

# Add bindable and drawable interfaces to include directories
target_include_directories(Renderer PUBLIC renderer/bindable)
target_include_directories(Renderer PUBLIC renderer/drawable)

# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(Renderer PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(Renderer PRIVATE ../include/renderer)
