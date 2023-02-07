#pragma once

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "models.grpc.pb.h"

#include "packet_stream.hpp"
#include <mutex>
#include <algorithm>


namespace iot_monitoring {
	class RemoteEndopintServer final : public models::RemoteEndpoint::Service {
	private:
		std::shared_ptr<std::vector<data::device_info>> _device_queue;
		std::shared_ptr<std::map<uint16_t, data::PacketStream>> _data_queue;
		std::mutex _m;
	public:
		explicit RemoteEndopintServer(std::shared_ptr<std::vector<data::device_info>>, std::shared_ptr<std::map<uint16_t, data::PacketStream>>);

		grpc::Status GetServerInfo(::grpc::ServerContext*, const ::models::Empty*, ::models::ServerInfo*);
		grpc::Status ListDevices(::grpc::ServerContext*, const ::models::Empty*, ::models::DeviceResponse*);
		grpc::Status ReadPacket(::grpc::ServerContext*, const ::models::PacketRequest*, ::grpc::ServerWriter<::models::Packet>*);
		grpc::Status ReadAll(::grpc::ServerContext*, ::grpc::ServerReaderWriter< ::models::Packet, ::models::ReadAllOn>*);

	};
}