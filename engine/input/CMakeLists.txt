#--------------------------------------------------------------------------
# input
#--------------------------------------------------------------------------
FILE(GLOB input_headers *.h)
FILE(GLOB input_sources *.cc)
SET(files_input	${input_headers} ${input_sources})

SOURCE_GROUP("input" FILES ${files_input})


SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(input STATIC ${files_input} ${files_pch})
TARGET_PCH(input ../)
ADD_DEPENDENCIES(input glfw)
TARGET_LINK_LIBRARIES(input PUBLIC engine glfw)