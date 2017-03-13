#ifndef __CLIENT_MGR_H__
#define __CLIENT_MGR_H__

#include <map>
#include "ClientHandler.h"
#include "Mutex.h"

namespace marz {

class ClientMgr : public NonCopyable {
public:
	ClientMgr();
	~ClientMgr();

	void AddHandler(uint32_t key, ClientHandler* handler);
	void DelHandler(uint32_t key);

	void SendMsgToRouteSvr(const Message* message);

private:
	std::map<uint32_t, ClientHandler*> client_map_;
	mars::Mutex mutex_;

};

} // namespace marz

#endif // __CLIENT_MGR_H__
