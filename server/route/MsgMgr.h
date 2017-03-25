#ifndef __MSG_MGR_H__
#define __MSG_MGR_H__

#include <map>
#include "MsgHandler.h"
#include "Mutex.h"

namespace marz {

class MsgMgr : public NonCopyable {
public:
	MsgMgr();
	~MsgMgr();

	void AddHandler(uint32_t key, MsgHandler* handler);
	void DelHandler(uint32_t key);

	void SendToMsgSvr(Packet* packet);

private:
	std::map<uint32_t, MsgHandler*> msg_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __MSG_MGR_H__
