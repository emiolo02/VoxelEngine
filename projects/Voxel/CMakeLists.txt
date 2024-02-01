#--------------------------------------------------------------------------
# Voxel project
#--------------------------------------------------------------------------

PROJECT(WaterTest)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)
FILE(GLOB example_shaders code/*.glsl)

SET(files_example ${example_headers} ${example_sources} ${example_shaders})
SOURCE_GROUP("Voxel" FILES ${files_example})

ADD_EXECUTABLE(Voxel ${files_example})
TARGET_LINK_LIBRARIES(Voxel core render)
ADD_DEPENDENCIES(Voxel core render)

if (MSVC)
	set_property(TARGET Voxel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()