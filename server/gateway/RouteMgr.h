#ifndef __ROUTE_MGR_H__
#define __ROUTE_MGR_H__

#include <map>
#include "ClientHandler.h"
#include "Mutex.h"

namespace marz {

class RouteMgr : public NonCopyable {
public:
	RouteMgr();
	~RouteMgr();

	void AddHandler(uint32_t key, RouteHandler* handler);
	void DelHandler(uint32_t key);

	void SendMsgToRouteSvr(int16_t service, int16_t command, int16_t sequence, const Message* message);

private:
	std::map<uint32_t, RouteHandler*> route_map_;
	mars::Mutex mutex_;

};

} // namespace marz

#endif // __ROUTE_MGR_H__
