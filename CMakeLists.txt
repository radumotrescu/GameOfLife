CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(SET CMP0091 NEW)
PROJECT(GameOfLife C CXX)

set(CMAKE_CXX_STANDARD 14)

# Initialize Conan #############################################################
INCLUDE(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

#========== non-conan dependencies =============#
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

#============ imgui
FILE(GLOB IMGUI_HEADERS "bindings/*.h")
FILE(GLOB IMGUI_SOURCES "bindings/*.cpp")

add_library(imguibind ${IMGUI_SOURCES})

FILE(GLOB MY_HEADERS "src/*.h")
FILE(GLOB MY_SOURCES "src/*.cpp")

set(files_all ${MY_SOURCES} ${MY_HEADERS})

# Build our project with the help of conan.
add_executable(GameOfLife ${files_all})
set_property (TARGET GameOfLife
  PROPERTY
    # Enable C++17 standard compliance
    CXX_STANDARD 17)
	
set_property (TARGET GameOfLife
  PROPERTY 
    # Enable /MD
    MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

#============= gl3w
set(glew_Includes "gl3w/include")
add_library(glew STATIC gl3w/src/gl3w.c)

include_directories(src gl3w/include bindings ${CONAN_INCLUDE_DIRS})
target_link_libraries(GameOfLife imguibind glew ${CONAN_LIBS})
		
# Now enable our tests
enable_testing()
add_subdirectory(test)