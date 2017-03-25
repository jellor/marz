#include "UserHandler.h"

#include "UserMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

UserHandler::UserHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

UserHandler::~UserHandler() {
	DLOG << "Route Handler Destructor ......";
}

void UserHandler::OnInactive(const ChannelPtr& channel_ptr) {
	UserMgr& user_mgr = Singleton<UserMgr>::GetInstance();
	user_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void UserHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	UserMgr& user_mgr = Singleton<UserMgr>::GetInstance();
	user_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&UserHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void UserHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void UserHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

} // namespace marz
