#include "UserMgr.h"

namespace marz {

UserMgr::UserMgr():
user_map_(),
mutex_()
{
}

UserMgr::~UserMgr() {
}

void UserMgr::AddHandler(uint32_t key, UserHandler* handler) {
	Lock lock(mutex_);

	user_map_.insert(std::make_pair(key, handler));
}

void UserMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	user_map_.erase(key);
}

void UserMgr::SendToUserSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = user_map_.begin(); it != user_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
