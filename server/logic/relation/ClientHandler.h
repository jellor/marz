#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include "PacketHandler.h"

namespace marz {

class ClientHandler : public PacketHandler {
public:
	ClientHandler(const ChannelPtr& channel_ptr);
	virtual ~ClientHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;
	virtual void OnActive(const ChannelPtr& channel_ptr) override;
	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

};

} // namespace marz

#endif // __CLIENT_HANDLER_H__
