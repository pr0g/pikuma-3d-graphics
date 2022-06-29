cmake_minimum_required(VERSION 3.15)

project(pikuma LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
  SDL2
  URL https://www.libsdl.org/release/SDL2-2.0.20.tar.gz
  URL_HASH MD5=a53acc02e1cca98c4123229069b67c9e)

FetchContent_MakeAvailable(SDL2)

add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME} PRIVATE src/main.c src/display.c src/math-types.c src/fps.c
                          src/mesh.c src/triangle.c src/array.c src/lighting.c)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,AppleClang,Clang>:
          -Weverything
          -Wall
          -Wextra
          -pedantic
          -Wno-sign-conversion;-Wno->
          $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall
          -Wextra
          -pedantic>)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

add_custom_target(
  run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
