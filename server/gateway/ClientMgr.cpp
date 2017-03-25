#include "ClientMgr.h"
#include "protocolbuffer/Im.Server.pb.h"

namespace marz {

ClientMgr::ClientMgr():
client_map_(),
addr_map_(),
mutex_()
{
}

ClientMgr::~ClientMgr() {
}

void ClientMgr::AddHandler(uint32_t key, ClientHandler* handler) {
	Lock lock(mutex_);

	client_map_.insert(std::make_pair(key, handler));
}

void ClientMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	client_map_.erase(key);
}

void ClientMgr::SendMsgToRouteSvr(const Message* message) {
	
	ClientHandler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = client_map_.begin(); it != client_map_.end(); it ++) {
			//it->second->NotifyBalanceSvr(notify);
		}
	}
}

void ClientMgr::AddHandler(const std::string& addr, ClientHandler* handler) {
	Lock lock(mutex_);

	addr_map_.insert(std::make_pair(addr, handler));
}

void ClientMgr::DelHandler(const std::string& addr) {
	Lock lock(mutex_);

	addr_map_.erase(addr);
}

void ClientMgr::SendToClient(Packet* packet) {

	std::string addr = IpAddress::ToString(packet->ip, packet->port);
	{
		Lock lock(mutex_);
		auto it = addr_map_.find(addr);
		if (it != addr_map_.end()) {
			it->second->SendPacket(packet);
		}
	}
	
}

} // namespace marz

