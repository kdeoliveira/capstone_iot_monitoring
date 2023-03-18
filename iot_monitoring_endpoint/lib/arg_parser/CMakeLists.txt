project ("arg_parser")

add_library(${PROJECT_NAME} STATIC "./src/arg_parser.cxx" "include/arg_parser.hpp" "include/utils.hpp")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")


