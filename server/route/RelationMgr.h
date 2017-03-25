#ifndef __RELATION_MGR_H__
#define __RELATION_MGR_H__

#include <map>
#include "RelationHandler.h"
#include "Mutex.h"

namespace marz {

class RelationMgr : public NonCopyable {
public:
	RelationMgr();
	~RelationMgr();

	void AddHandler(uint32_t key, RelationHandler* handler);
	void DelHandler(uint32_t key);

	void SendToRelationSvr(Packet* packet);

private:
	std::map<uint32_t, RelationHandler*> relation_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __RELATION_MGR_H__
