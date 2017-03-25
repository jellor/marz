#include "SessionMgr.h"

namespace marz {

SessionMgr::SessionMgr():
session_map_(),
mutex_()
{
}

SessionMgr::~SessionMgr() {
}

void SessionMgr::AddHandler(uint32_t key, SessionHandler* handler) {
	Lock lock(mutex_);

	session_map_.insert(std::make_pair(key, handler));
}

void SessionMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	session_map_.erase(key);
}

void SessionMgr::SendToSessionSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = session_map_.begin(); it != session_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
