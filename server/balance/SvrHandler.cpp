#include "SvrHandler.h"

#include "protocolbuffer/Im.Server.pb.h"

#include "SvrMgr.h"
#include "Singleton.h"

using namespace Im;

namespace marz {

SvrHandler::SvrHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1)
{
	DLOG << "Svr Handler Constructor ......";
}

SvrHandler::~SvrHandler() {
	DLOG << "Svr Handler Destructor ......";
}

void SvrHandler::OnInactive(const ChannelPtr& channel_ptr) {
	SvrMgr& svr_mgr = Singleton<SvrMgr>::GetInstance();
	svr_mgr.DeleteServer(id_);

	Close(channel_ptr);
}

void SvrHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
}

void SvrHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		case Im::Base::HEARTBEAT:
			break;
		case Im::Base::MSG_SERVER_INFO:
			HandleServer(channel_ptr, packet);
			break;
		case Im::Base::USER_COUNT_UPDATE:
			HandleServerUpdate(channel_ptr, packet);
			break;
		case Im::Base::REQ_MSG_SERVER:
			HandleServerRequest(channel_ptr, packet);
			break;
		default:
			WLOG << "Wrong Packet, Command -> " << packet->command;
			break;
	}
}

void SvrHandler::HandleServer(const ChannelPtr& channel_ptr, Packet* packet) {
	
	server_t* server = new server_t;

	Im::Server::ServerInfo msg;
	msg.ParseFromArray(packet->data, packet->length - 18);

	server->host_name = msg.host_name();
	server->ip_addr_1 = msg.ip_1();
	server->ip_addr_2 = msg.ip_2();
	server->port = msg.port();
	server->cur_connect_cnt = msg.cur_connect_cnt();
	server->max_connect_cnt = msg.max_connect_cnt();

	DLOG << msg.host_name() << " | "<< msg.ip_1() << " | " << msg.ip_2() << " | " << msg.port() << " | " 
		<< msg.cur_connect_cnt() << " | " << msg.max_connect_cnt();

	SvrMgr& svr_mgr = Singleton<SvrMgr>::GetInstance();
	svr_mgr.InsertServer(id_, server);

	delete packet;
}

void SvrHandler::HandleServerUpdate(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Server::ServerUpdateNotity notify;
	notify.ParseFromArray(packet->data, packet->length - 18);

	SvrMgr& svr_mgr = Singleton<SvrMgr>::GetInstance();

	// 1: increment, 2: decrement
	uint32_t action = notify.action();
	switch (action) {
		case 1:
			// increment
			svr_mgr.Increment(id_, 1);
			break;
		case 2:
			// decrement
			svr_mgr.Decrement(id_, 1);
			break;
		default:
			DLOG << "Update Server Fialed";
			break;
	}

	delete packet;
}

void SvrHandler::HandleServerRequest(const ChannelPtr& channel_ptr, Packet* packet) {
	
	Im::Server::ServerRequest request;
	request.ParseFromArray(packet->data, packet->length - 18);

	SvrMgr& svr_mgr = Singleton<SvrMgr>::GetInstance();

	Im::Server::ServerResponse* response = new Im::Server::ServerResponse();

	if (svr_mgr.GetServerCnt() == 0) {
		response->set_result_type(Im::Base::REFUSE_REASON_NO_SERVER);
		SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_MSG_SERVER, packet->sequence, response, channel_ptr);
	} else {
		const server_t* server = svr_mgr.GetServer();
		if (server == NULL) {	
			response->set_result_type(Im::Base::REFUSE_REASON_MSG_SERVER_FULL);
			SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_MSG_SERVER, packet->sequence, response, channel_ptr);
		} else {
			response->set_result_type(Im::Base::REFUSE_REASON_NONE);
			response->set_priority(server->ip_addr_1);
			response->set_backup(server->ip_addr_2);
			response->set_port(server->port);
			SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_MSG_SERVER, packet->sequence, response, channel_ptr);
		}
	}

	delete response;
	delete packet;
}

} // namespace marz
