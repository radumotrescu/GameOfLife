CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(GameOfLife-test CXX)

file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE TEST_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
set(files_all ${TEST_SOURCE} ${TEST_HEADER})


add_executable(GameOfLife_test ${files_all})
target_link_libraries(GameOfLife_test ${CONAN_LIBS})

# Include Encryptor test #######################################################
ENABLE_TESTING()
ADD_TEST(NAME test
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND ${PROJECT_NAME})