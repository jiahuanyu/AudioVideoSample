# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jni-libs ${CMAKE_SOURCE_DIR}/../jniLibs)
set(lib-name native-lib)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/util
        ${CMAKE_SOURCE_DIR}/player
        ${CMAKE_SOURCE_DIR}/player/decoder
        ${CMAKE_SOURCE_DIR}/player/render)

link_directories(${jni-libs}/${ANDROID_ABI})

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/player/*.cpp
        ${CMAKE_SOURCE_DIR}/player/decoder/*.cpp
        ${CMAKE_SOURCE_DIR}/player/render/*.cpp)

add_library(${lib-name}
        SHARED
        ${src-files})

set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil)

set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z)

target_link_libraries(${lib-name}
        ${log-lib}
        ${third-party-libs}
        ${native-libs})