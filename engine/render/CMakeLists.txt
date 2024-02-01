#--------------------------------------------------------------------------
# render
#--------------------------------------------------------------------------

SET(files_render_display
	window.h
	window.cc
	)

SOURCE_GROUP("display" FILES ${files_render_display})

FILE(GLOB graphics_headers graphics/*.h)
FILE(GLOB graphics_sources graphics/*.cc)

SET(files_render_graphics ${graphics_headers} ${graphics_sources})

SOURCE_GROUP("graphics" FILES ${files_render_graphics})

SET(files_render_debug
	Debug.h
	Debug.cc
	)

SOURCE_GROUP("debug" FILES ${files_render_debug})

SET(files_render
	${files_render_display}
	${files_render_graphics}
	${files_render_debug}
	${files_render_pch}
	)

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(render STATIC ${files_render} ${files_pch})
TARGET_PCH(render ../)
ADD_DEPENDENCIES(render exts imgui glew glfw glm_static)
TARGET_LINK_LIBRARIES(render PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS} glm_static)