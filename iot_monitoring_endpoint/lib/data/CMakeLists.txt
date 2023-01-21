project("data")

add_library(${PROJECT_NAME} STATIC "src/data.cxx")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")