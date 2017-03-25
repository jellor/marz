#ifndef __RELATION_HANDLER_H__
#define __RELATION_HANDLER_H__

#include "PacketHandler.h"
#include "TimerId.h"

namespace marz {

class RelationHandler : public PacketHandler {
public:
	RelationHandler(const ChannelPtr& channel_ptr);
	virtual ~RelationHandler();

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

#endif // __RELATION_HANDLER_H__
