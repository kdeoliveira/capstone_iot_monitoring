#pragma once

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "models.grpc.pb.h"

#include <future>

namespace iot_monitoring {
	class RemoteEndopintServer final : public models::RemoteEndpoint::Service {
	public:
		explicit RemoteEndopintServer();

		grpc::Status GetServerInfo(::grpc::ServerContext*, const ::models::Empty*, ::models::ServerInfo*);

		static std::future<void> start_server(std::launch mode, const std::string& addr);
	};
}