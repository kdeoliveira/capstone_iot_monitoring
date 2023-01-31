#include "server.hpp"
#include "remoteendpoint.hpp"


namespace iot_monitoring {


	RemoteEndopintServer::RemoteEndopintServer(std::shared_ptr<std::vector<data::device_info>> device_q, std::shared_ptr<std::map<std::string, data::PacketStream>> data_q) {
		this->_data_queue = data_q;
		this->_device_queue = device_q;
	}

	grpc::Status RemoteEndopintServer::GetServerInfo(::grpc::ServerContext* context, const ::models::Empty* request, ::models::ServerInfo* response) {
		response->set_major(1);
		response->set_minor(1);
		response->set_rpc(0);

		return grpc::Status::OK;
	}

	static std::unique_ptr<grpc::Server> server;

	void start_server(std::shared_ptr<std::vector<data::device_info>> dev_info, std::shared_ptr<std::map<std::string, data::PacketStream>> data_queue, std::launch mode, const std::string& addr) {
		
		grpc::EnableDefaultHealthCheckService(true);

		RemoteEndopintServer service(dev_info, data_queue);

		grpc::ServerBuilder builder;
		builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		server = builder.BuildAndStart();
		
		//Function to asynchronously shutdown server should be passed
		server->Wait();
	}

	void shutdown_server()
	{
		if (server.get() != nullptr)
			server->Shutdown();
	}

	grpc::Status RemoteEndopintServer::ListDevices(::grpc::ServerContext*, const::models::Empty*, ::models::DeviceResponse* response)
	{
		auto devices = response->mutable_device();
		response->set_size(this->_device_queue->size());
		if (response->size() == 0)
			return grpc::Status::Status(grpc::StatusCode::NOT_FOUND, "No device found");

		std::unique_lock<std::mutex> l{ this->_m };

		std::transform(this->_device_queue->cbegin(), this->_device_queue->cend(), devices->begin(), [&](const iot_monitoring::data::device_info& info) {
			auto d = ::models::Device();
			d.set_id(info.id);
			d.set_hardware_id(info.hardware_id);
			d.set_status(static_cast<::models::STATUS>(info.hardware_status));
			return d;
			});


		return grpc::Status();
	}

	grpc::Status RemoteEndopintServer::ReadPacket(::grpc::ServerContext*, const::models::PacketRequest*, ::grpc::ServerWriter<::models::Packet>*)
	{
		return grpc::Status();
	}

	template<typename T>
	void list_devices(std::function<void(T&)> fn)
	{

	}
}



