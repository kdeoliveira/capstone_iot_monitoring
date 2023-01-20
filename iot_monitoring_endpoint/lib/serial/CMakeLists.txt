project ("serial")

add_library(${PROJECT_NAME} STATIC "src/device.cxx" "include/serial.hpp" "src/serial.cxx")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")
