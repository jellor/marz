#ifndef __MSG_HANDLER_H__
#define __MSG_HANDLER_H__


#include "PacketHandler.h"

namespace marz {

class MsgHandler : public PacketHandler {
public:
	MsgHandler(const ChannelPtr& channel_ptr);
	virtual ~MsgHandler();

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

#endif // __MSG_HANDLER_H__
