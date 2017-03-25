#ifndef __CLIENT_MGR_H__
#define __CLIENT_MGR_H__

#include <map>
#include <string>
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

	void AddHandler(const std::string& addr, ClientHandler* handler);
	void DelHandler(const std::string& addr);

	void SendToClient(Packet* packet);

private:
	std::map<uint32_t, ClientHandler*> client_map_;
	std::map<std::string, ClientHandler*> addr_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __CLIENT_MGR_H__
