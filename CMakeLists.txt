cmake_minimum_required(VERSION 3.5)
project(Voxel VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

add_subdirectory(Libs)

add_library(common INTERFACE Common/Config.hpp)
target_include_directories(common INTERFACE common/)
target_link_libraries(common INTERFACE libs)

add_subdirectory(Engine)
#target_link_libraries(engine INTERFACE common)

file(GLOB_RECURSE sources Projects/*.cpp)
add_executable(vox ${sources})
target_link_libraries(vox engine)
target_include_directories(vox PUBLIC src)

if (MSVC)
    set_property(TARGET vox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set_property(TARGET vox PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
else ()
    set_target_properties(vox
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
endif ()
