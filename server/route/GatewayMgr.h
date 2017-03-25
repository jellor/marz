#ifndef __GATEWAY_MGR_H__
#define __GATEWAY_MGR_H__

#include <map>
#include "GatewayHandler.h"
#include "Mutex.h"

namespace marz {

class GatewayMgr : public NonCopyable {
public:
	GatewayMgr();
	~GatewayMgr();

	void AddHandler(const std::string& ip_addr, GatewayHandler* handler);
	void DelHandler(const std::string& ip_addr);

	void SendToGatewaySvr(Packet* packet);

private:
	std::map<std::string, GatewayHandler*> gateway_map_;
	Mutex mutex_;

};

} // namespace marz

#endif // __CLIENT_MGR_H__
