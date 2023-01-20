# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("iot_monitoring_endpoint")

# Include sub-projects.
add_subdirectory ("iot_monitoring_endpoint")
add_subdirectory ("lib")



# REQUIRES:
# Visual C++ compiler
# choco install ninja
# choco install nasm