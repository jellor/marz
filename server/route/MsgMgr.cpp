#include "MsgMgr.h"

namespace marz {

MsgMgr::MsgMgr():
msg_map_(),
mutex_()
{
}

MsgMgr::~MsgMgr() {
}

void MsgMgr::AddHandler(uint32_t key, MsgHandler* handler) {
	Lock lock(mutex_);

	msg_map_.insert(std::make_pair(key, handler));
}

void MsgMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	msg_map_.erase(key);
}

void MsgMgr::SendToMsgSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = msg_map_.begin(); it != msg_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
