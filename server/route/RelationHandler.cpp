#include "RelationHandler.h"

#include "RelationMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

RelationHandler::RelationHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

RelationHandler::~RelationHandler() {
	DLOG << "Route Handler Destructor ......";
}

void RelationHandler::OnInactive(const ChannelPtr& channel_ptr) {
	RelationMgr& relation_mgr = Singleton<RelationMgr>::GetInstance();
	relation_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void RelationHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	RelationMgr& relation_mgr = Singleton<RelationMgr>::GetInstance();
	relation_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&RelationHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void RelationHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void RelationHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

} // namespace marz
