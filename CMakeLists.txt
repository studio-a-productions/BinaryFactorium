cmake_minimum_required(VERSION 3.20)

# desktop edition
project(BinaryFactorium LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# all into bin bin bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# only single config respect this
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()
set(CMAKE_DEFAULT_BUILD_TYPE "Release" CACHE STRING "Default config for multi-config generators")


include(FetchContent)

set(SDL_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3 
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.4.10
    GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(SDL3)


file(GLOB_RECURSE BINF_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE BINF_LIB CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
)

add_executable(BinaryFactorium 
    ${BINF_SRC} 
    ${BINF_LIB}
)

target_include_directories(BinaryFactorium PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(BinaryFactorium PRIVATE 
    BINF_PLATFORM=DESKTOP_SDL
    BINF_NOWARNS
)

target_link_libraries(BinaryFactorium PRIVATE SDL3::SDL3)
