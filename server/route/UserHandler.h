#ifndef __USER_HANDLER_H__
#define __USER_HANDLER_H__


#include "PacketHandler.h"
#include "TimerId.h"

namespace marz {

class UserHandler : public PacketHandler {
public:
	UserHandler(const ChannelPtr& channel_ptr);
	virtual ~UserHandler();

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

#endif // __USER_HANDLER_H__
