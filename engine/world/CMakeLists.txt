#--------------------------------------------------------------------------
# world
#--------------------------------------------------------------------------
FILE(GLOB world_headers *.h)
FILE(GLOB world_sources *.cc)
SET(files_world	${world_headers} ${world_sources})

SOURCE_GROUP("world" FILES ${files_world})


SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(world STATIC ${files_world} ${files_pch})
TARGET_PCH(world ../)
ADD_DEPENDENCIES(world glfw)
TARGET_LINK_LIBRARIES(world PUBLIC engine glfw)