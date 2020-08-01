cmake_minimum_required(VERSION 3.9)
project(PathTracer LANGUAGES CXX)

set(APP_NAME PathTracer)
add_executable(${APP_NAME} src/main.cpp)
target_compile_features(${APP_NAME} PRIVATE cxx_std_17)
