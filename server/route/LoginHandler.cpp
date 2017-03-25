#include "protocolbuffer/Im.Login.pb.h"
#include "GatewayMgr.h"
#include "LoginHandler.h"
#include "LoginMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

namespace marz {

LoginHandler::LoginHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false),
timer_id_()
{
	DLOG << "Route Handler Constructor ......";
}

LoginHandler::~LoginHandler() {
	DLOG << "Route Handler Destructor ......";
}

void LoginHandler::OnInactive(const ChannelPtr& channel_ptr) {
	LoginMgr& login_mgr = Singleton<LoginMgr>::GetInstance();
	login_mgr.DelHandler(id_);

	channel_ptr->GetEventLoop()->RemoveTimer(timer_id_);
	Close(channel_ptr);

}

void LoginHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();
	LoginMgr& login_mgr = Singleton<LoginMgr>::GetInstance();
	login_mgr.AddHandler(id_, this);

	timer_id_ = channel_ptr->GetEventLoop()->AddTimer(std::bind(&LoginHandler::OnTimeout, this), Timestamp::Now().Macrosecond(), 5, 1);
}

void LoginHandler::OnTimeout() {
	WLOG << "OnTimeout";
}

void LoginHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
	switch (packet->service) {
		case Im::Base::SERVICE_LOGIN:
			HandleLoginSvr(channel_ptr, packet);
			break;
		default:
			WLOG << "Wrong Packet, command -> " << packet->command;
			break;
	}
}

void LoginHandler::HandleLoginSvr(const ChannelPtr& channel_ptr, Packet* packet) {
	GatewayMgr& gateway_mgr = Singleton<GatewayMgr>::GetInstance();
	gateway_mgr.SendToGatewaySvr(packet);
}

} // namespace marz
