#include "BalanceHandler.h"

#include "BalanceMgr.h"
#include "Singleton.h"
#include "Timestamp.h"

extern std::atomic<uint64_t> g_max_connect_num;
extern std::atomic<uint64_t> g_cur_connect_num;

extern std::string g_client_listen_host;
extern uint16_t real_client_listen_port;

namespace marz {

BalanceHandler::BalanceHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
master_id_(false)
{
	DLOG << "Balance Handler Constructor ......";
}

BalanceHandler::~BalanceHandler() {
	DLOG << "Balance Handler Destructor ......";
}

void BalanceHandler::OnInactive(const ChannelPtr& channel_ptr) {
	BalanceMgr& balance_mgr = Singleton<BalanceMgr>::GetInstance();
	balance_mgr.DelHandler(id_);
	Close(channel_ptr);
}

void BalanceHandler::OnActive(const ChannelPtr& channel_ptr) {
	id_ = channel_ptr->GetSocket()->Fd();

	BalanceMgr& balance_mgr = Singleton<BalanceMgr>::GetInstance();
	balance_mgr.AddHandler(id_, this);
	channel_ptr->GetEventLoop()->AddTimer(std::bind(&BalanceHandler::onTimeout, this), Timestamp::now().macrosecond(), 5, 1);
	SendServerInfo(channel_ptr);
}

void BalanceHandler::SendServerInfo(const ChannelPtr& channel_ptr) {
	Im::Server::ServerInfo msg;

	char hostname[256] = { 0 };
	gethostname(hostname, 256);

	msg.set_ip_1(g_client_listen_host);
	msg.set_ip_2(g_client_listen_host);
	msg.set_port(real_client_listen_port);
	msg.set_host_name(hostname);
	msg.set_max_connect_cnt(g_max_connect_num);
	msg.set_cur_connect_cnt(g_cur_connect_num);

	SendMessage(Im::Base::SERVICE_OTHER, Im::Base::MSG_SERVER_INFO, 1, &msg, channel_ptr);
}

void BalanceHandler::NotifyBalanceSvr(const Im::Server::ServerUpdateNotity& notify) {
	SendMessage(Im::Base::SERVICE_OTHER, Im::Base::USER_COUNT_UPDATE, 1, &notity, channel_ptr_);
}

void BalanceHandler::OnTimeout(){
	WARN << "onTimeout";
}

void BalanceHandler::onMessage(const ChannelPtr& channel_ptr, Packet* packet) {

}

} // namespace marz
