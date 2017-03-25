#include "GroupMgr.h"

namespace marz {

GroupMgr::GroupMgr():
group_map_(),
mutex_()
{
}

GroupMgr::~GroupMgr() {
}

void GroupMgr::AddHandler(uint32_t key, GroupHandler* handler) {
	Lock lock(mutex_);

	group_map_.insert(std::make_pair(key, handler));
}

void GroupMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	group_map_.erase(key);
}

void GroupMgr::SendToGroupSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = group_map_.begin(); it != group_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
