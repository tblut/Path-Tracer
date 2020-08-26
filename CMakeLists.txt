cmake_minimum_required(VERSION 3.9)
project(PathTracer LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})

set(APP_NAME PathTracer)
add_executable(${APP_NAME} ${SOURCE_FILES})
target_compile_features(${APP_NAME} PRIVATE cxx_std_17)
if(MSVC)
	target_compile_options(${APP_NAME} PUBLIC /MP /GR- /Oi /fp:fast)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result)
if(ipo_result)
	set_target_properties(${APP_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
