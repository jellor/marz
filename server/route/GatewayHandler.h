#ifndef __GATEWAY_HANDLER_H__
#define __GATEWAY_HANDLER_H__

#include "PacketHandler.h"

namespace marz {

class GatewayHandler : public PacketHandler {
public:
	GatewayHandler(const ChannelPtr& channel_ptr);
	virtual ~GatewayHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void OnTimeout();

	void SendToGatewaySvr(Packet* packet);

private:

	void HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet);

	void HandleLogin(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleGroup(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleMessage(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleRelation(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleSession(const ChannelPtr& channel_ptr, Packet *packet);

	void HandleUser(const ChannelPtr& channel_ptr, Packet* packet);


	std::string peer_addr_;
	std::set<std::string> addr_set_;
};

} // namespace marz

#endif // __CLIENT_HANDLER_H__
