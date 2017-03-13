#ifndef __SVR_HANDLER_H__
#define __SVR_HANDLER_H__

#include "PacketHandler.h"

namespace marz {

class SvrHandler : public PacketHandler {
public:
	SvrHandler(const ChannelPtr& channel_ptr);
	virtual ~SvrHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;
	virtual void OnActive(const ChannelPtr& channel_ptr) override;
	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

private:

	void HandleServer(const ChannelPtr& channel_ptr, Packet* packet);
	void HandleServerUpdate(const ChannelPtr& channel_ptr, Packet* packet);
	void HandleServerRequest(const ChannelPtr& channel_ptr, Packet* packet);

	int64_t id_;

};

} // namespace marz

#endif // __SVR_HANDLER_H__
