#ifndef __ROUTE_HANDLER_H__
#define __ROUTE_HANDLER_H__

#include "PacketHandler.h"

namespace marz {

class RouteHandler : public PacketHandler {
public:
	RouteHandler(const ChannelPtr& channel_ptr);
	virtual ~RouteHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void OnTimeout();

	void HandleRouteSvr(Packet* packet);

private:
	int64_t id_;
	bool master_id_;
	TimerId timer_id_;

};

} // namespace marz

#endif // __ROUTE_HANDLER_H__
