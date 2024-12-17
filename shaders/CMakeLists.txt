# Set the shaders directory
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders")

# Create the output directory for shaders
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Function to compile a shader
function(compile_shader IN_FILE MODEL ENTRY_POINT OUT_FILE)
    add_custom_command(
        OUTPUT ${OUT_FILE}
        COMMAND fxc.exe /T ${MODEL} /E ${ENTRY_POINT} /Fo ${OUT_FILE} ${IN_FILE}
        DEPENDS ${IN_FILE}
        COMMENT "Compiling shader: ${IN_FILE}"
        VERBATIM
    )
endfunction()

# Function to extract the first extension of the file
function(get_type FILE_EXTENSION OUT_VAR)
    # Match the first extension
    string(REGEX MATCH "\\.([^.]+)" FIRST_EXTENSION_MATCH "${FILE_EXTENSION}")
    # Remove the leading dot from the matched extension
    string(REGEX REPLACE "^\\.([^.]*)$" "\\1" FIRST_EXTENSION "${FIRST_EXTENSION_MATCH}")
    # Ensure the extension is in lowercase
    string(TOLOWER ${FIRST_EXTENSION} FIRST_EXTENSION_MATCH)
    # Return the result via the output variable
    set(${OUT_VAR} "${FIRST_EXTENSION}" PARENT_SCOPE)
endfunction()

# Find all HLSL shader files
file(GLOB HLSL_SHADERS "${SHADER_SOURCE_DIR}/*.hlsl")

# Add shaders to a Visual Studio folder
source_group("Shaders" FILES ${HLSL_SHADERS})

# Loop through shaders and create compile commands
foreach(SHADER ${HLSL_SHADERS})
    # Extract the name of the file without extensions
    cmake_path(GET SHADER STEM SHADER_FILENAME)
    # Extract the extension
    cmake_path(GET SHADER EXTENSION SHADER_EXTENSION)

    # Create the output path for the compiled shader
    set(COMPILED_SHADER "${SHADER_OUTPUT_DIR}/${SHADER_FILENAME}.cso")
    # Extract the shader type from the extension
    get_type(${SHADER_EXTENSION} SHADER_TYPE)

    if(SHADER_TYPE STREQUAL "vs")      # Vertex shader: processes vertices
        set(SHADER_MODEL "vs_5_0")
    elseif(SHADER_TYPE STREQUAL "ps")  # Pixel/Fragment shader: colors pixels
        set(SHADER_MODEL "ps_5_0")
    elseif(SHADER_TYPE STREQUAL "gs")  # Geometry shader: modifies primitives
        set(SHADER_MODEL "gs_5_0")
    elseif(SHADER_TYPE STREQUAL "hs")  # Hull shader: controls tesselation
        set(SHADER_MODEL "hs_5_0")
    elseif(SHADER_TYPE STREQUAL "ds")  # Domain shader: finalizes tesselation
        set(SHADER_MODEL "ds_5_0")
    elseif(SHADER_TYPE STREQUAL "cs")  # Compute shader: general-purpose computation
        set(SHADER_MODEL "cs_5_0")
    else()
        set(SHADER_MODEL "unknown")  # Unknown shader type, causes build failure
    endif()

    # Call the function to compile the shader
    compile_shader(${SHADER} ${SHADER_MODEL} "main" ${COMPILED_SHADER})
    # Add the compiled shader to the application dependencies
    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})

    message(STATUS "${SHADER_FILENAME} compilation successful.")
endforeach()

# Add a custom target to build shaders
add_custom_target(Shaders ALL DEPENDS ${COMPILED_SHADERS})

# Make shaders appear in the Visual Studio project under the Shaders folder
add_custom_target(ShaderSources SOURCES ${HLSL_SHADERS})
