cmake_minimum_required(VERSION 3.9)
project(PathTracer LANGUAGES CXX)

set(APP_NAME PathTracer)
add_executable(${APP_NAME} src/main.cpp)
target_compile_features(${APP_NAME} PRIVATE cxx_std_17)
if(MSVC)
	target_compile_options(${APP_NAME} PUBLIC /MP /GR- /Oi /fp:fast)
endif()
