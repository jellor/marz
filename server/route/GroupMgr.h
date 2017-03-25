#ifndef __GROUP_MGR_H__
#define __GROUP_MGR_H__

#include <map>
#include "GroupHandler.h"
#include "Mutex.h"

namespace marz {

class GroupMgr : public NonCopyable {
public:
	GroupMgr();
	~GroupMgr();

	void AddHandler(uint32_t key, GroupHandler* handler);
	void DelHandler(uint32_t key);

	void SendToGroupSvr(Packet* packet);

private:
	std::map<uint32_t, GroupHandler*> group_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __GROUP_MGR_H__
