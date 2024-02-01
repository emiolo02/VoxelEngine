# Requirements
1. CMake 3.2+.
2. Compiler for C++17
  
# Build instructions
Create a "build" directory and run cmake with that as its target.

# Usage
The base engine contains two classes, Window and App. Override the App class for your project to implement your own update loop. The Window class is used to open a window and connect input handlers to it.

Input handlers can be connected by assigning a [lambda function](https://www.geeksforgeeks.org/lambda-expression-in-c/) to a callback matching the input you require. It is possible to have more than one window open at a time, although one must select which window to render to using the function called Window::MakeCurrent prior to doing any GL calls.

_IMPORTANT NOTICE_: The environment uses [precompiled headers](https://en.wikipedia.org/wiki/Precompiled_header), which means ***all*** source files (.cc, .cpp) must include "config.h".

# Example
The example project is simply an example implementation which displays a colored triangle on a grey background.

# External libraries

This environment uses:
* GLFW
    * Used for window and OpenGL context handling, input handling, etc.
* GLEW - OpenGL Extension Wrangler Library
    * Provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform. OpenGL core and extension functionality is exposed in a single header file.
* Dear ImGui
    * A slim, simple to use, immediate mode graphical user interface.
* GLM
    * Simple to use vector math library.

# Additional Help

### CMake

https://cmake.org/cmake/help/latest/guide/tutorial/index.html