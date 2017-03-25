#ifndef __SESSION_MGR_H__
#define __SESSION_MGR_H__

#include <map>
#include "SessionHandler.h"
#include "Mutex.h"

namespace marz {

class SessionMgr : public NonCopyable {
public:
	SessionMgr();
	~SessionMgr();

	void AddHandler(uint32_t key, SessionHandler* handler);
	void DelHandler(uint32_t key);

	void SendToSessionSvr(Packet* packet);

private:
	std::map<uint32_t, SessionHandler*> session_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __SESSION_MGR_H__
