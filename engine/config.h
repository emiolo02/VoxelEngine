//------------------------------------------------------------------------------
/**
    @file core/config.h
    
	Main configure file for types and OS-specific stuff.
	
	(C) 2015-2020 See the LICENSE file.
*/
#pragma once
#ifdef __WIN32__
#include "win32/pch.h"
#endif

#include <stdint.h>
#include <atomic>
#include <xmmintrin.h>
#include <assert.h>
#include <memory.h>
#include <string>
#include <vector>
#include <sstream>

//#define GLM_FORCE_SSE42 // or GLM_FORCE_SSE42 if your processor supports it
//#define GLM_FORCE_ALIGNED
#include <vec3.hpp> // glm::vec3
#include <vec4.hpp> // glm::vec4
#include <mat4x4.hpp> // glm::mat4
#include "gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "gtx/transform.hpp"
#include "gtc/quaternion.hpp"

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::ivec3 vec3i;
typedef glm::vec4 vec4;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

typedef glm::quat quat;

typedef uint32_t	uint32;
typedef int32_t		int32;
typedef uint16_t	uint16;
typedef int16_t		int16;
typedef uint8_t		uint8;
typedef int8_t		int8;
typedef uint8_t		uchar;

// eh, windows already defines byte, so don't redefine byte if we are running windows
#ifndef __WIN32__
typedef uint8_t      byte;
#endif

typedef uint8_t		ubyte;
typedef float		float32;
typedef double		float64;

#define j_min(x, y) x < y ? x : y
#define j_max(x, y) x > y ? x : y
#define PI 3.14159265358979323846
#ifdef NULL
#undef NULL
#define NULL nullptr
#endif

inline mat4 rotationaxis(vec3 const& v)
{
	return 
		glm::rotate(v.x, vec3(1, 0, 0)) *
		glm::rotate(v.y, vec3(0, 1, 0)) *
		glm::rotate(v.z, vec3(0, 0, 1));
}

inline mat4 rotationx(float const& rad)
{
	return glm::rotate(rad, vec3(1, 0, 0));
}

inline mat4 rotationy(float const& rad)
{
	return glm::rotate(rad, vec3(0, 1, 0));
}

inline mat4 rotationz(float const& rad)
{
	return glm::rotate(rad, vec3(0, 0, 1));
}

inline bool
SameDirection(vec3 const& a, vec3 const& b)
{
	return glm::dot(a, b) > 0;
}

inline void 
split(std::string& str, char separator, std::vector<std::string>& out)
{
	// create a stream from the string  
	std::stringstream s(str);

	std::string s2;
	while (getline(s, s2, separator))
	{
		out.push_back(s2); // store the string in s2  
	}
}