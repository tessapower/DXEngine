# Add all shader files found in this directory to the SHADERS variable
file (GLOB SHADERS *.hlsl)

add_custom_target(
	Shaders
	SOURCES
	${SHADERS}
)
