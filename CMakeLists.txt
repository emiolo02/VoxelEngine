#--------------------------------------------------------------------------
# gscept laboration environment
#--------------------------------------------------------------------------
cmake_policy(SET CMP0048 NEW) # set policy for older versions
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(Voxel_engine VERSION 1.0)

OPTION(CI_MODE "Run in CI mode" OFF) # Disabled by default

IF(CI_MODE)
    ADD_DEFINITIONS(-DCI_TEST)
ENDIF(CI_MODE)

MACRO(TARGET_PCH target path)
IF(WIN32)
	IF(MSVC)
		SET_TARGET_PROPERTIES(${target}
			PROPERTIES 
			COMPILE_FLAGS "/Yuconfig.h /Fp${CMAKE_CFG_INTDIR}/${target}.pch")
		SET_SOURCE_FILES_PROPERTIES(${path}config.cc
            PROPERTIES
            COMPILE_FLAGS "/Ycconfig.h")
	ENDIF()
ENDIF()
ENDMACRO(TARGET_PCH)

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
ENDIF()

IF(MSVC)
    SET(OPENGL_LIBS opengl32.lib)
	    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
ELSE()
    SET(OPENGL_LIBS GL X11 Xxf86vm pthread Xrandr Xi Xinerama Xcursor)
ENDIF()

SET(GSCEPT_LAB_ENV_ROOT ${CMAKE_CURRENT_DIR})

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/bin>)

SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS GLEW_STATIC)
ADD_SUBDIRECTORY(exts)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(projects)
