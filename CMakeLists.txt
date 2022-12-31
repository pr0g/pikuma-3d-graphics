cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0135 NEW)

project(pikuma LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
  SDL2
  URL https://www.libsdl.org/release/SDL2-2.0.20.tar.gz
  URL_HASH MD5=a53acc02e1cca98c4123229069b67c9e)
FetchContent_Declare(
  as-c-math
  GIT_REPOSITORY https://github.com/pr0g/as-c-math.git
  GIT_TAG 7d9786335b0049ec9d6cd3622c7510360255b6f3)
FetchContent_Declare(
  upng
  GIT_REPOSITORY https://github.com/pr0g/upng.git
  GIT_TAG edb284b15fc3c37260d27182e4cf3a366c1734bf)

FetchContent_MakeAvailable(SDL2 as-c-math upng)

add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE src/main.c
          src/display.c
          src/fps.c
          src/mesh.c
          src/triangle.c
          src/array.c
          src/lighting.c
          src/texture.c
          src/camera.c
          src/frustum.c
          src/polygon.c)
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
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main
                                              as-c-math upng)

add_custom_target(
  run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if(WIN32)
  # copy the SDL2.dll to the same folder as the executable
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)
endif()
