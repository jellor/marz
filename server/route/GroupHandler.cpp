#include "GroupHandler.h"
#include "GroupMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

GroupHandler::GroupHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

GroupHandler::~GroupHandler() {
	DLOG << "Route Handler Destructor ......";
}

void GroupHandler::OnInactive(const ChannelPtr& channel_ptr) {
	GroupMgr& group_mgr = Singleton<GroupMgr>::GetInstance();
	group_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void GroupHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	GroupMgr& group_mgr = Singleton<GroupMgr>::GetInstance();
	group_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&GroupHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void GroupHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void GroupHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		
	}
}

} // namespace marz
