cmake_minimum_required(VERSION 3.9)
project(PathTracer LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Common functions
include(CheckIPOSupported)
function(enable_ipo TARGET)
	check_ipo_supported(RESULT ipo_result)
	if(ipo_result)
		set_target_properties(${TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
		set_target_properties(${TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
	endif()
endfunction()

function(add_test_exe TEST_NAME EXE_NAME SOURCES)
	add_executable(${EXE_NAME} ${SOURCES})
	target_link_libraries(${EXE_NAME} PRIVATE PathTracerLib Catch2)
	set_target_properties(${EXE_NAME} PROPERTIES FOLDER "Tests")
	add_test(${TEST_NAME} ${EXE_NAME})
	enable_ipo(${EXE_NAME})
endfunction()

# Main application
file(GLOB_RECURSE SOURCE_FILES "src/*.h" "src/*.cpp")
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/main.cpp") # Remove main from common lib
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_FILES})

add_library(PathTracerLib STATIC ${SOURCE_FILES})
target_include_directories(PathTracerLib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_features(PathTracerLib PUBLIC cxx_std_17)
if(MSVC)
	include(CheckAvxSupport)
    check_avx_support()
	target_compile_options(PathTracerLib PUBLIC /MP /GR- /Oi /fp:fast ${AVX_FLAGS})
else()
	target_compile_options(PathTracerLib PUBLIC -ffast-math -march=native)
	target_link_libraries(PathTracerLib PUBLIC pthread)
endif()
enable_ipo(PathTracerLib)

add_library(stb_image_write INTERFACE)
target_include_directories(stb_image_write INTERFACE ${CMAKE_SOURCE_DIR}/extern/stb/include)
target_link_libraries(PathTracerLib PRIVATE stb_image_write)

add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_link_libraries(PathTracerLib PUBLIC json)

add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE ${CMAKE_SOURCE_DIR}/extern/tinyobjloader/include)
target_link_libraries(PathTracerLib PRIVATE tinyobjloader)

add_executable(PathTracer src/main.cpp)
target_link_libraries(PathTracer PRIVATE PathTracerLib)
enable_ipo(PathTracer)

# Unit tests
enable_testing()
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CMAKE_SOURCE_DIR}/extern/catch2/include)

add_test_exe(math MathTests "tests/MathTests.cpp")
add_test_exe(sampler SamplerTests "tests/SamplerTests.cpp")
add_test_exe(bsdf BsdfTests "tests/BsdfTests.cpp")
add_test_exe(shape ShapeTests "tests/ShapeTests.cpp")
add_test_exe(helper HelperTests "tests/HelperTests.cpp")
