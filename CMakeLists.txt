cmake_minimum_required(VERSION 3.5)
project(Voxel VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

add_subdirectory(libs)

add_library(common INTERFACE common/pch.hpp)
target_include_directories(common INTERFACE common)

file(GLOB_RECURSE sources src/*.cpp)
add_executable(vox ${sources})
target_precompile_headers(vox PUBLIC common/pch.hpp)
target_link_libraries(vox libs common)
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
