#include "GatewayMgr.h"

namespace marz {

GatewayMgr::GatewayMgr():
gateway_map_(),
mutex_()
{
}

GatewayMgr::~GatewayMgr() {
}

void GatewayMgr::AddHandler(const std::string& addr, GatewayHandler* handler) {
	Lock lock(mutex_);

	gateway_map_.insert(std::make_pair(addr, handler));
}

void GatewayMgr::DelHandler(const std::string& addr) {
	Lock lock(mutex_);

	gateway_map_.erase(addr);
}

void GatewayMgr::SendToGatewaySvr(Packet* packet) {
	
	Lock lock(mutex_);
	for (auto it = gateway_map_.begin(); it != gateway_map_.end(); it ++) {
		it->second->SendToGatewaySvr(packet);
	}
	
}

} // namespace marz

