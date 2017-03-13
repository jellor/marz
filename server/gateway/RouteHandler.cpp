#include "RouteHandler.h"

#include "UserInfoMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

RouteHandler::RouteHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr)
id_(-1),
master_id_(false)
{
	DEBUG << "Route Handler Constructor ......";
}

RouteHandler::~RouteHandler() {
	DEBUG << "Route Handler Destructor ......";
}

void RouteHandler::OnInactive(const ChannelPtr& channel_ptr) {
	RouteMgr& route_mgr = Singleton<RouteMgr>::GetInstance();
	route_mgr.DelHandler(id_);

	Close(channel_ptr);
}

void RouteHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	RouteMgr& route_mgr = Singleton<UserInfoMgr>::getInstance();
	route_mgr.AddHandler(id_, this);

	channel_ptr->GetEventLoop()->AddTimer(std::bind(&RouteHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void RouteHandler::OnTimeout() {
	WARN << "OnTimeout";
}

void RouteHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
		case Im::Base::HEARTBEAT:
			break;
		case Im::Base::ONLINE_USER_INFO:
			
			break;
		case Im::Base::USER_INFO_UPDATE:
			
			break;
		case Im::Base::ROLE_SET:
			
			break;
		case Im::Base::REQ_USERS_STATUS:
			
			break;
		case Im::Base::MSG_DATA:
		case Im::Base::P2P_CMD:
		case Im::Base::MSG_READ_NOTIFY:
		case Im::Base::SERVER_KICK_USER:
		case Im::Base::CHANGE_MEMBER_NOTIFY:
		case Im::Base::REQ_REMOVE_SESSION:
			
			break;
		default:
			WARN << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

void RouteHandler::SendMsgToRouteSvr(int16_t service, int16_t command, int16_t sequence, const Message* message) {
	RouteHandler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = route_map_.begin(); it != route_map_.end(); it ++) {
			SendMessage(service, command, sequence, message);
		}
	}
}

} // namespace marz
