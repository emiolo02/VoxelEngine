add_library(libs INTERFACE)

set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)
add_subdirectory(glew)
#set_target_properties(libglew_static PROPERTIES FOLDER libs)
target_include_directories(libs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glew/include)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)
target_include_directories(libs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)

file(GLOB imgui_sources
        imgui/*.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${imgui_sources})

target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC imgui PUBLIC imgui/backends)

#add_subdirectory(openal)
#target_include_directories(libs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/openal/include)

add_subdirectory(glm)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory(assimp)

add_library(stb_image INTERFACE stb/stb_image.h)
target_include_directories(libs INTERFACE stb)

target_link_libraries(libs INTERFACE
        libglew_static
        glfw
        imgui
        #OpenAL::OpenAL
        glm::glm
        assimp::assimp
        stb_image
)
