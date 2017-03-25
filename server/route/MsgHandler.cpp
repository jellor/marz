#include "MsgHandler.h"

#include "MsgMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

MsgHandler::MsgHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

MsgHandler::~MsgHandler() {
	DLOG << "Route Handler Destructor ......";
}

void MsgHandler::OnInactive(const ChannelPtr& channel_ptr) {
	MsgMgr& msg_mgr = Singleton<MsgMgr>::GetInstance();
	msg_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void MsgHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	MsgMgr& msg_mgr = Singleton<MsgMgr>::GetInstance();
	msg_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&MsgHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void MsgHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void MsgHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
	
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

} // namespace marz
