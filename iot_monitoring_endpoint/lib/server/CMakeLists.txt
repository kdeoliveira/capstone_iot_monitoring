cmake_minimum_required(VERSION 3.15)

project("server")


add_library(${PROJECT_NAME} STATIC "src/server.cxx" "./include/server.hpp")

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} PRIVATE "./def")
target_include_directories(${PROJECT_NAME} PRIVATE "./include")

target_include_directories(${PROJECT_NAME} INTERFACE "./include")


option(GRPC_REMOTE "Build grpc via fetchContent" OFF)

if(GRPC_REMOTE)
	#libprotobuf is included in the git library
include(FetchContent)
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG        v1.50.1
    )
  FetchContent_MakeAvailable(grpc)

  # Since FetchContent uses add_subdirectory under the hood, we can use
  # the grpc targets directly from this build.
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
  set(_GRPC_GRPCPP grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
  endif()

else()
    set(VCPKG_TARGET_TRIPLET x64-windows-triplet)

	set(protobuf_MODULE_COMPATIBLE TRUE)
  find_package(Protobuf CONFIG REQUIRED)
  message(STATUS "Using protobuf ${Protobuf_VERSION}")

  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
  set(_REFLECTION gRPC::grpc++_reflection)
  if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
  else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  endif()


  find_package(gRPC CONFIG REQUIRED)
  message(STATUS "Using gRPC ${gRPC_VERSION}")

  set(_GRPC_GRPCPP gRPC::grpc++)
  if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
  endif()
	

endif()

#include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# PROTOBUF FILES
#get_filename_component(model_proto "./proto/models.proto" ABSOLUTE)
#get_filename_component(model_proto_path "${model_proto}" PATH)

add_library(model_proto "def/models.pb.cc" "def/models.pb.h" "def/models.grpc.pb.cc" "def/models.grpc.pb.h" "def/remoteendpoint.hpp")

target_link_libraries(model_proto ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})

# Protobuf generate
# protoc -I. --grpc_out=..\include --cpp_out=..\include --plugin=protoc-gen-grpc=C:\Users\deolivek\source\repos\iot_monitoring_endpoint\out\build\x64-Debug\_deps\grpc-build\grpc_cpp_plugin.exe models.proto



target_link_libraries(${PROJECT_NAME} PRIVATE model_proto ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})


# GRPC:
# https://chromium.googlesource.com/external/github.com/grpc/grpc/+/HEAD/src/cpp/README.md