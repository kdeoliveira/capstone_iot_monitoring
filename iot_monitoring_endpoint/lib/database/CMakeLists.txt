cmake_minimum_required(VERSION 3.16)

project("database")

add_library(${PROJECT_NAME} STATIC "./src/database.cxx")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")



set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()


# Enforce the C++ standard, and disable extensions.
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()



find_package(bsoncxx CONFIG REQUIRED)
    

find_package(mongocxx CONFIG REQUIRED)



include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})

find_package(Boost REQUIRED)
if (CMAKE_VERSION VERSION_LESS 3.15.0)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE BSONCXX_POLY_USE_BOOST=ON)



target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)