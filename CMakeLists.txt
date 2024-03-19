cmake_minimum_required(VERSION 3.17)
project(cppsafe-tidy VERSION 0.1.0)

include(CTest)
enable_testing()

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# cpp options
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
	add_compile_options(-Wall)
	add_compile_options(-Wextra)
	add_compile_options(-Werror)
	add_compile_options(-Wno-unused-parameter)
	add_compile_options(-Wno-missing-field-initializers)
endif()

if((CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
endif()


# profile cpp options
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
	add_compile_options($<$<CONFIG:Debug>:-fsanitize=undefined>)
	add_link_options($<$<CONFIG:Debug>:-fsanitize=undefined>)
	message(STATUS "Enable ubsan")
endif()

# cpp std
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# Package finders
if(NOT LOCAL_CLANG)
	message(FATAL "LOCAL_CLANG is not defined, please set it to llvm install dir")
endif()

set(Clang_DIR "${LOCAL_CLANG}/lib/cmake/clang/")
find_package(Clang REQUIRED CONFIG)

file(GLOB SRCS src/*.cpp)
add_library(cppsafe-tidy MODULE ${SRCS})

target_compile_options(cppsafe-tidy PRIVATE -fno-rtti)
target_include_directories(cppsafe-tidy PRIVATE
	${CLANG_INCLUDE_DIRS})
target_link_libraries(cppsafe-tidy PRIVATE clangTidyUtils)
