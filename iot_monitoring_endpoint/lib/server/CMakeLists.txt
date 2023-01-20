cmake_minimum_required(VERSION 3.15)

project("server")


add_library(${PROJECT_NAME} STATIC "./src/server.cxx" "include/server.hpp")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")

#libprotobuf is included in the git library
include(FetchContent)
FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.50.1
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)

set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

#include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# PROTOBUF FILES
#get_filename_component(model_proto "./proto/models.proto" ABSOLUTE)
# get_filename_component(model_proto_path "${model_proto}" PATH)

add_library(model_proto "include/models.pb.cc" "include/models.pb.h" "include/models.grpc.pb.cc" "include/models.grpc.pb.h" "include/server.hpp")

target_link_libraries(model_proto grpc++ libprotobuf grpc++_reflection)

# Protobuf generate
# protoc -I. --grpc_out=..\include --cpp_out=..\include --plugin=protoc-gen-grpc=C:\Users\deolivek\source\repos\iot_monitoring_endpoint\out\build\x64-Debug\_deps\grpc-build\grpc_cpp_plugin.exe models.proto



target_link_libraries(${PROJECT_NAME} PUBLIC model_proto grpc++ libprotobuf grpc++_reflection)


# GRPC:
# https://chromium.googlesource.com/external/github.com/grpc/grpc/+/HEAD/src/cpp/README.md