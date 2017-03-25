#include "SessionHandler.h"

#include "SessionMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

SessionHandler::SessionHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

SessionHandler::~SessionHandler() {
	DLOG << "Route Handler Destructor ......";
}

void SessionHandler::OnInactive(const ChannelPtr& channel_ptr) {
	SessionMgr& session_mgr = Singleton<SessionMgr>::GetInstance();
	session_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void SessionHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	SessionMgr& session_mgr = Singleton<SessionMgr>::GetInstance();
	session_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&SessionHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void SessionHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void SessionHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

} // namespace marz
