#ifndef __ROUTE_HANDLER_H__
#define __ROUTE_HANDLER_H__

#include "protobuf/Im.Other.pb.h"
#include "protobuf/Im.Server.pb.h"
#include "protobuf/Im.Login.pb.h"
#include "protobuf/Im.Buddy.pb.h"

#include "PacketHandler.h"
#include "HandlerChain.h"

using namespace mars;

class RouteHandler : public PacketHandler {
public:
	RouteHandler(const ChannelPtr& channel_ptr);
	virtual ~RouteHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void OnTimeout();
	void SendMsgToRouteSvr(int16_t service, int16_t command, int16_t sequence, const Message* message);

private:
	int64_t id_;
	bool master_id_;

};

#endif