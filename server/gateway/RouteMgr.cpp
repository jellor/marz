#include "RouteMgr.h"
#include "protobuf/Im.Server.pb.h"

namespace marz {

RouteMgr::RouteMgr():
route_map_(),
mutex_()
{
}

RouteMgr::~RouteMgr() {
}

void RouteMgr::AddHandler(uint32_t key, RouteHandler* handler) {
	Lock lock(mutex_);

	route_map_.insert(std::make_pair(key, handler));
}

void RouteMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	route_map_.erase(key);
}

void RouteMgr::SendMsgToRouteSvr(int16_t service, int16_t command, int16_t sequence, const Message* message) {
	
	RouteHandler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = route_map_.begin(); it != route_map_.end(); it ++) {
			it->second->SendMsgToRouteSvr(message);
		}
	}
}

} // namespace marz

