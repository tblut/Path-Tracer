cmake_minimum_required(VERSION 3.9)
project(PathTracer LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
list(REMOVE_ITEM SOURCE_FILES "src/main.cpp") # Remove main from common lib
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})

add_library(PathTracerLib STATIC ${SOURCE_FILES})
target_include_directories(PathTracerLib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_features(PathTracerLib PUBLIC cxx_std_17)
if(MSVC)
	target_compile_options(PathTracerLib PUBLIC /MP /GR- /Oi /fp:fast)
endif()

add_library(stb_image_write INTERFACE)
target_include_directories(stb_image_write INTERFACE ${CMAKE_SOURCE_DIR}/extern/stb/include)
target_link_libraries(PathTracerLib PRIVATE stb_image_write)

add_executable(PathTracer src/main.cpp)
target_link_libraries(PathTracer PRIVATE PathTracerLib)

# Unit tests
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CMAKE_SOURCE_DIR}/extern/catch2/include)

add_executable(MathTests "tests/MathTests.cpp")
target_link_libraries(MathTests PRIVATE PathTracerLib Catch2)
set_property(TARGET MathTests PROPERTY FOLDER "Tests") 

# Enable link time optimizations
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result)
if(ipo_result)
	set_target_properties(PathTracerLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	set_target_properties(PathTracerLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
	set_target_properties(PathTracer PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	set_target_properties(PathTracer PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
	set_target_properties(MathTests PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	set_target_properties(MathTests PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
endif()
