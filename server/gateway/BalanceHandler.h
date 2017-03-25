#ifndef __BALANCE_HANDLER_H__
#define __BALANCE_HANDLER_H__

#include "protocolbuffer/Im.Server.pb.h"
#include "PacketHandler.h"

namespace marz {

class BalanceHandler : public PacketHandler {
public:
	BalanceHandler(const ChannelPtr& channel_ptr);
	virtual ~BalanceHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;
	virtual void OnActive(const ChannelPtr& channel_ptr) override;
	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void OnTimeout();

	void NotifyBalanceSvr(const Im::Server::ServerUpdateNotity& notify);

private:
	void SendServerInfo(const ChannelPtr& channel_ptr);

	int64_t id_;
	bool master_id_;
};

} // namespace marz

#endif // __BALANCE_HANDLER_H__
