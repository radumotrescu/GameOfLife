# MathLibHelper

Based on: https://github.com/ssloy/tinyrenderer

Uses SDL2 for rendering the pixels, no OpenGL.

SDL2 used from Conan:
https://github.com/bincrafters/conan-sdl2

**HOW TO RUN**

----------
mkdir build && cd build

----------
conan install .. --profile ../vs2017

--------
cmake .. -G "Visual Studio 15 Win64"


-----

cmake --build .