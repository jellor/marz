#ifndef __USER_MGR_H__
#define __USER_MGR_H__

#include <map>
#include "UserHandler.h"
#include "Mutex.h"

namespace marz {

class UserMgr : public NonCopyable {
public:
	UserMgr();
	~UserMgr();

	void AddHandler(uint32_t key, UserHandler* handler);
	void DelHandler(uint32_t key);

	void SendToUserSvr(Packet* packet);

private:
	std::map<uint32_t, UserHandler*> user_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __USER_MGR_H__
