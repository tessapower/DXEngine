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
    "${PROJECT_SOURCE_DIR}/include/Game.h"
    "${PROJECT_SOURCE_DIR}/include/Keyboard.h"
    "${PROJECT_SOURCE_DIR}/include/message_map.h"
    "${PROJECT_SOURCE_DIR}/include/renderer.h"
    "${PROJECT_SOURCE_DIR}/include/resource.h"
    "${PROJECT_SOURCE_DIR}/include/stdafx.h"
)

add_library(engine STATIC
    ${SOURCES}
    ${HEADERS}
)


# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(engine PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(engine PRIVATE ../include/)
