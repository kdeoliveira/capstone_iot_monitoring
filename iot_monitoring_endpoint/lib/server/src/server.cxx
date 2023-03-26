#include "server.hpp"
#include "remoteendpoint.hpp"

namespace iot_monitoring {


	RemoteEndopintServer::RemoteEndopintServer(std::shared_ptr<std::vector<data::device_info>> device_q, std::shared_ptr<std::map<uint16_t, data::PacketStream>> data_q) {
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

	void start_server(std::shared_ptr<std::vector<data::device_info>> dev_info, std::shared_ptr<std::map<uint16_t, data::PacketStream>> data_queue, std::launch mode, const std::string& addr) {
		
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
		//auto devices = response->mutable_device();
		//response->set_size(this->_device_queue->size());
		//if (response->size() == 0)
		//	return grpc::Status::Status(grpc::StatusCode::NOT_FOUND, "No device found");

		//std::unique_lock<std::mutex> l{ this->_m };

		//std::transform(this->_device_queue->cbegin(), this->_device_queue->cend(), devices->begin(), [&](const iot_monitoring::data::device_info& info) {
		//	auto d = ::models::Device();
		//	d.set_id(info.id);
		//	d.set_hardware_id(info.hardware_id);
		//	d.set_status(static_cast<::models::STATUS>(info.hardware_status));
		//	return d;
		//	});
		
		response->set_size(10);
		for (int i = 0; i < 10; i++) {
			auto dev = response->add_device();
			dev->set_id(i + 10);
			dev->set_hardware_id("Hardware id");
			dev->set_status(models::STATUS::ON);
		}

		return grpc::Status();
	}

	grpc::Status RemoteEndopintServer::ReadPacket(::grpc::ServerContext* context, const::models::PacketRequest* request, ::grpc::ServerWriter<::models::Packet>* response)
	{
		
		auto header_id = request->id();
		std::unique_lock<std::mutex>l{ this->_m };

		auto packets = this->_data_queue->find(header_id);
		if (packets == this->_data_queue->end()) {
			return grpc::Status::Status(grpc::StatusCode::NOT_FOUND, "No packets found");
		}

		::models::Packet out = ::models::Packet::Packet();

		while (!context->IsCancelled() && !packets->second.empty()) {
			
			auto pck = packets->second.pop();
			out.set_id((int32_t)pck.header.id);
			out.set_data(std::to_string(pck.payload.first));
			auto time_now = std::chrono::system_clock::now().time_since_epoch();
			out.set_timestamp((int64_t)time_now.count());

			response->Write(out);
		}
		

		return grpc::Status();
	}

	grpc::Status RemoteEndopintServer::ReadAll(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::models::Packet, ::models::ReadAllOn>* stream)
	{
		std::unique_lock<std::mutex>l{ this->_m };

		
		if (this->_data_queue->empty()) {
			return grpc::Status::Status(grpc::StatusCode::NOT_FOUND, "No packets found");
		}

		

		::models::ReadAllOn cancellation;

		while (!context->IsCancelled() && stream->Read(&cancellation)) {

			if (!cancellation.active())
				break;

			for (auto packets = this->_data_queue->begin(); packets != this->_data_queue->end(); ++packets) {
				//packets->second.wait();
				if (packets->second.empty())
					continue;

				::models::Packet out = ::models::Packet::Packet();

				auto pck = packets->second.pop();
				out.set_id((int32_t)pck.header.id);
				if (pck.header.id == data::GPS) {
					out.set_data(std::to_string(pck.payload.first) + "," + std::to_string(pck.payload.second));
					if (pck.payload.first < -90.00 || pck.payload.second < -90.00)
						out.set_type(models::TYPE::ERR);
					else
						out.set_type(models::TYPE::STRING);
				}
				else {
					out.set_data(std::to_string(pck.payload.first)+","+ std::to_string(pck.payload.second));
					if (pck.quality == 0xFF)
						out.set_type(models::TYPE::ERR);
					else
						out.set_type(models::TYPE::STRING);
				}


				
				auto time_now = std::chrono::system_clock::now().time_since_epoch();
				out.set_timestamp((int64_t)time_now.count());
				stream->Write(std::move(out));
			}
			
			//Sleep(10);
		}
		
		return grpc::Status();
	}

	template<typename T>
	void list_devices(std::function<void(T&)> fn)
	{

	}
}



