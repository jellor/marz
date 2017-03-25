#ifndef __LOGIN_MGR_H__
#define __LOGIN_MGR_H__

#include <map>
#include "LoginHandler.h"
#include "Mutex.h"

namespace marz {

class LoginMgr : public NonCopyable {
public:
	LoginMgr();
	~LoginMgr();

	void AddHandler(uint32_t key, LoginHandler* handler);
	void DelHandler(uint32_t key);

	void SendToLoginSvr(Packet* packet);

private:
	std::map<uint32_t, LoginHandler*> login_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __LOGIN_MGR_H__
