#include "models.grpc.pb.h"
#include <grpcpp/grpcpp.h>

#include "server.hpp"

namespace iot_monitoring{
	RemoteEndopintServer::RemoteEndopintServer(){}

	grpc::Status RemoteEndopintServer::GetServerInfo(::grpc::ServerContext* context, const ::models::Empty* request, ::models::ServerInfo* response) {
		response->set_major(1);
		response->set_minor(1);
		response->set_rpc(0);
		auto ver = ::grpc::Version();
		return grpc::Status::OK;
	}

	std::future<void> RemoteEndopintServer::start_server(std::launch mode, const std::string& addr) {
		RemoteEndopintServer service = RemoteEndopintServer();
		grpc::ServerBuilder builder;
		builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		return std::async(mode, [&]() {
			server->Wait();
			});
	}
}
