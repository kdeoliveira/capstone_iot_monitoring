# CMakeList.txt : CMake project for iot_monitoring_endpoint, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (iot_monitoring_endpoint "iot_monitoring_endpoint.cpp" "iot_monitoring_endpoint.h"  )


target_link_libraries(${PROJECT_NAME} PUBLIC arg_parser)
target_link_libraries(${PROJECT_NAME} PUBLIC serial)
target_link_libraries(${PROJECT_NAME} PUBLIC server)
target_link_libraries(${PROJECT_NAME} PUBLIC data)