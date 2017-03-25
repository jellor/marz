#include "RelationMgr.h"

namespace marz {

RelationMgr::RelationMgr():
relation_map_(),
mutex_()
{
}

RelationMgr::~RelationMgr() {
}

void RelationMgr::AddHandler(uint32_t key, RelationHandler* handler) {
	Lock lock(mutex_);

	relation_map_.insert(std::make_pair(key, handler));
}

void RelationMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	relation_map_.erase(key);
}

void RelationMgr::SendToRelationSvr(Packet* packet) {
	
	Handler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = relation_map_.begin(); it != relation_map_.end(); it ++) {
			it->second->SendPacket(packet);
		}
	}
}

} // namespace marz
