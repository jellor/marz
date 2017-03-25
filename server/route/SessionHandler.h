#ifndef __SESSION_HANDLER_H__
#define __SESSION_HANDLER_H__

#include "PacketHandler.h"
#include "TimerId.h"

namespace marz {

class SessionHandler : public PacketHandler {
public:
	SessionHandler(const ChannelPtr& channel_ptr);
	virtual ~SessionHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void OnTimeout();

private:
	int64_t id_;
	bool master_id_;
	TimerId timer_id_;

};

} // name marz

#endif // __SESSION_HANDLER_H__
