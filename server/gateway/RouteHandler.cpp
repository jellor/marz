#include "RouteHandler.h"
#include "RouteMgr.h"
#include "ClientMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

RouteHandler::RouteHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

RouteHandler::~RouteHandler() {
	DLOG << "Route Handler Destructor ......";
}

void RouteHandler::OnInactive(const ChannelPtr& channel_ptr) {
	RouteMgr& route_mgr = Singleton<RouteMgr>::GetInstance();
	route_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void RouteHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	RouteMgr& route_mgr = Singleton<RouteMgr>::GetInstance();
	route_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&RouteHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, - 1);
}

void RouteHandler::OnTimeout() {
	//WLOG << "OnTimeout";
}

void RouteHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->command) {
	
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

void RouteHandler::HandleRouteSvr(Packet* packet) {
	ClientMgr& client_mgr = Singleton<ClientMgr>::GetInstance();
	client_mgr.SendToClient(packet);
}

} // namespace marz
