#include <signal.h>
#include "BalanceHandler.h"
#include "RouteHandler.h"
#include "ClientHandler.h"
#include "IpParser.h"
#include "Config.h"
#include "Logger.h"
#include "StringSplit.h"

using namespace std;
namespace marz {

std::atomic<uint64_t> g_max_connect_num;
std::atomic<uint64_t> g_cur_connect_num;

std::atomic<uint64_t> g_msg_cnt_per_sec;

std::string g_client_listen_host; // = NULL;

uint16_t real_client_listen_port = 0;
uint16_t real_console_listen_port = 0;
uint16_t real_balance_port = 0;
uint16_t real_route_port = 0;

class GatewaySvr : public Svr {
public:
	GatewaySvr() {
		DEBUG << "Balance Service Constructor ......";
	}

	~GatewaySvr() {
		DEBUG << "Balance Service Destructor ......";
	}

	virtual Handler* CreateHandler(const ChannelPtr& channel_ptr) override {

		Handler* handler = NULL;

        if ((channel_ptr->GetLocalAddress().Port() == real_client_listen_port)) {
            handler = new ClientHandler(channel_ptr);
        } else if ((channel_ptr->GetLocalAddress().Port() == real_console_listen_port)) {
        	// handler = new ConsoleHandler();
        } else if ((channel_ptr->GetPeerAddress().port() == real_balance_port)) {
            handler = new BalanceHandler(channel_ptr);
        } else if ((channel_ptr->getPeerAddress().port() == real_route_port)) {
            handler = new RouteHandler(channel_ptr);
        } else {
        	WLOG << "Create Handler Failed";
        }

        return handler;
	}
};

} // namespace marz

using namespace marz;

int main(int argc, char* argv[]) {
	signal(SIGPIPE, SIG_IGN);

	Config config("gateway_svr.conf");
	
	const char* client_listen_hosts = config.getValueByName("ClientListenHosts");
	const char* client_listen_port = config.getValueByName("ClientListenPort");
	const char* console_listen_hosts = config.getValueByName("ConsoleListenHosts");
	const char* console_listen_port = config.getValueByName("ConsoleListenPort");
	const char* balance_hosts = config.getValueByName("BalanceHosts");
	const char* balance_port = config.getValueByName("BalancePort");
	const char* route_hosts = config.getValueByName("RouteHosts");
	const char* route_port = config.getValueByName("RoutePort");

	g_msg_cnt_per_sec = 0;

    g_cur_connect_num = 0;
	g_max_connect_num = atoi(config.getValueByName("MaxConnectCnt"));

	if (client_listen_hosts == NULL || client_listen_port == NULL 
		|| console_listen_hosts == NULL || console_listen_port == NULL
		|| balance_hosts == NULL || balance_port == NULL 
		|| route_hosts == NULL || route_port == NULL || db_hosts == NULL || db_port == NULL) {
		WARN << "Error : gateway_svr.conf Failed";
		WARN << client_listen_hosts << " " << client_listen_port 
			<< " " << console_listen_hosts << " " << console_listen_port
			<< " " << balance_hosts << " " << balance_port
			<< " " << route_hosts << " " << route_port;
		return 1;
	} else {
		DEBUG << client_listen_hosts << " " << client_listen_port 
			<< " " << console_listen_hosts << " " << console_listen_port
			<< " " << balance_hosts << " " << balance_port
			<< " " << route_hosts << " " << route_port;
	}

	real_client_listen_port = atoi(client_listen_port);
	real_console_listen_port = atoi(console_listen_port);
	real_balance_port = atoi(balance_port);
	real_route_port = atoi(route_port);

	std::vector<IpAddress> listen_hosts;
	std::vector<IpAddress> connect_hosts;

	mars::StringSplit client_listen_host_list(client_listen_hosts, ';');
	for (int i = 0; i < client_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(client_listen_host_list.GetItem(i), real_client_listen_port);
		listen_hosts.push_back(host);
		//TODO
		g_client_listen_host = client_listen_host_list.GetItem(i); 
	}

	mars::StringSplit console_listen_host_list(console_listen_hosts, ';');
	for (int i = 0; i < console_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(console_listen_host_list.GetItem(i), real_console_listen_port);
		listen_hosts.push_back(host);
	}

	mars::StringSplit balance_host_list(balance_hosts, ';');
	for (int i = 0; i < balance_host_list.GetItemCount(); i ++) {
		IpAddress host(balance_host_list.GetItem(i), real_balance_port);
		connect_hosts.push_back(host);
	}

	mars::StringSplit route_host_list(route_hosts, ';');
	for (int i = 0; i < route_host_list.GetItemCount(); i ++) {
		IpAddress host(route_host_list.GetItem(i), real_route_port);
		connect_hosts.push_back(host);
	}

	GatewaySvr* server = new GatewaySvr();

	DLOG << listen_hosts.size() << " " << connect_hosts.size();

	server->SetMainThread(listen_hosts, connect_hosts);
	// server->SetAcceptThread(address_list, accept_thread_cnt);
	server->SetIoThread(2);
	// server->SetConnectThread(address_list) 

	server->Start();

	delete server;
	DEBUG << "stop";
	return 0;
}

