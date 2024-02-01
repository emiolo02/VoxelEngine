#--------------------------------------------------------------------------
# physics
#--------------------------------------------------------------------------

FILE(GLOB physics_headers *.h)
FILE(GLOB physics_sources *.cc)

SET(files_physics ${physics_headers} ${physics_sources})

SOURCE_GROUP("physics" FILES ${files_physics})

SET(physics_files
	${files_physics}
	${files_pch}
	)

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(physics STATIC ${physics_files} ${files_pch})
TARGET_PCH(physics ../)
ADD_DEPENDENCIES(physics exts imgui glew glfw)
TARGET_LINK_LIBRARIES(physics PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS})