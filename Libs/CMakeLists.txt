include(FetchContent)

function(ensure_installed name url)
    FetchContent_Declare(
            ${name}
            URL ${url}
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${name}
            DOWNLOAD_EXTRACT_TIMESTAMP true
    )

    FetchContent_Populate(${name})

endfunction()


add_library(libs INTERFACE)

### glew ###

ensure_installed(glew https://github.com/Perlmint/glew-cmake/archive/refs/tags/glew-cmake-2.2.0.zip)

set(glew-cmake_BUILD_SHARED OFF)
set(ONLY_LIBS ON)

add_subdirectory(glew)
target_link_libraries(libs INTERFACE libglew_static)

### glfw ###

ensure_installed(glfw https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory(glfw)
target_include_directories(libs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)

target_link_libraries(libs INTERFACE glfw)

### imgui ###

ensure_installed(imgui https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.zip)

file(GLOB imgui_sources
        imgui/*.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${imgui_sources})

target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC imgui PUBLIC imgui/backends)

target_link_libraries(libs INTERFACE imgui)

### glm ###

ensure_installed(glm https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.7z)

add_subdirectory(glm)
target_include_directories(glm INTERFACE glm/)

target_link_libraries(libs INTERFACE glm::glm)

### assimp ###

ensure_installed(assimp https://github.com/assimp/assimp/archive/refs/tags/v5.4.3.zip)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)

add_subdirectory(assimp)

target_link_libraries(libs INTERFACE assimp::assimp)

### stb ###

ensure_installed(stb https://github.com/nothings/stb/archive/refs/heads/master.zip)

add_library(stb_image INTERFACE stb/stb_image.h)
target_include_directories(stb_image INTERFACE stb)

target_link_libraries(libs INTERFACE stb_image)
