#ifndef __LOGIN_HANDLER_H__
#define __LOGIN_HANDLER_H__

#include "PacketHandler.h"
#include "TimerId.h"

namespace marz {

class LoginHandler : public PacketHandler {
public:
	LoginHandler(const ChannelPtr& channel_ptr);
	virtual ~LoginHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

	void HandleLoginSvr(const ChannelPtr& channel_ptr, Packet* packet);
	
	void OnTimeout();

private:
	int64_t id_;
	bool master_id_;
	TimerId timer_id_;

};

} // name marz

#endif // __LOGIN_HANDLER_H__
