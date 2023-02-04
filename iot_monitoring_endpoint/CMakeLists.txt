# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("iot_monitoring_endpoint")

# Include sub-projects.
add_subdirectory ("iot_monitoring_endpoint")
add_subdirectory ("lib")

# == Compiler Definitions ==
# Define warning and complementary flags for x86 adn arms compilers
set(gcc_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# This library is used to define Compiler flags to be used across all build
add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

set_property(TARGET compiler_flags PROPERTY CXX_STANDARD 17)

target_compile_options(compiler_flags INTERFACE "$<${gcc_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>" "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>")

# REQUIRES:
# Visual C++ compiler
# choco install ninja

# choco install nasm
