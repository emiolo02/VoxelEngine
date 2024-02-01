#--------------------------------------------------------------------------
# Example project
#--------------------------------------------------------------------------

PROJECT(ExampleApp)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)
FILE(GLOB example_shaders code/*.glsl)

SET(files_example ${example_headers} ${example_sources} ${example_shaders})
SOURCE_GROUP("ExampleApp" FILES ${files_example})

ADD_EXECUTABLE(ExampleApp ${files_example})
TARGET_LINK_LIBRARIES(ExampleApp core render)
ADD_DEPENDENCIES(ExampleApp core render)

if (MSVC)
	set_property(TARGET ExampleApp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()