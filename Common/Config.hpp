#pragma once

#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "geometric.hpp"
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/string_cast.hpp"
#include "glm.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::string string;

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

using namespace glm;

// typedef glm::vec2 vec2;
// typedef glm::vec3 vec3;
// typedef glm::vec4 vec4;
// typedef glm::mat3 mat3;
// typedef glm::mat4 mat4;

inline uint32
Flatten(const ivec3 &position, const ivec3 &dimensions) {
  return position.x + dimensions.x * (position.y + dimensions.y * position.z);
}


// Disables copy and assign constructors.
#define NON_COPYABLE(ClassName)                                                \
  ClassName(const ClassName&) = delete;                                        \
  ClassName(ClassName&&) = delete;                                             \
  const ClassName& operator=(const ClassName&) = delete;                       \
  void operator=(ClassName&&) = delete;

constexpr float fPI = 3.14159265358979323846f;
constexpr float dPI = 3.14159265358979323846;

// Deletes all copy and assign constructors,
// as well as setting default constructor and destructor to private.
// Also provides a Get() function.
#define SINGLETON(ClassName)                                                   \
public:                                                                        \
  ClassName(const ClassName&) = delete;                                        \
  ClassName(ClassName&&) = delete;                                             \
  const ClassName& operator=(const ClassName&) = delete;                       \
  void operator=(ClassName&&) = delete;                                        \
  inline static ClassName& Get()                                               \
  {                                                                            \
    static ClassName instance;                                                 \
    return instance;                                                           \
  }                                                                            \
                                                                               \
private:                                                                       \
  ClassName() {}                                                               \
  ~ClassName() {}

#define DEBUG
#ifdef DEBUG
constexpr bool debugBuild = true;
#else
constexpr bool debugBuild = false;
#endif // DEBUG

#define LOG(msg)                                                               \
  if (debugBuild)                                                              \
    std::cout << msg << '\n';

#define PANIC(msg)                                                             \
  std::cout << msg << '\n';                                                    \
  exit(-1);
