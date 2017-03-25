#include "LoginMgr.h"

namespace marz {

LoginMgr::LoginMgr():
login_map_(),
mutex_()
{
}

LoginMgr::~LoginMgr() {
}

void LoginMgr::AddHandler(uint32_t key, LoginHandler* handler) {
	Lock lock(mutex_);

	login_map_.insert(std::make_pair(key, handler));
}

void LoginMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	login_map_.erase(key);
}

void LoginMgr::SendToLoginSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = login_map_.begin(); it != login_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
