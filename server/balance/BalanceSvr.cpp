#include <signal.h>
#include "Svr.h"
#include "SvrHandler.h"
#include "ClientHandler.h"
#include "IpParser.h"
#include "Config.h"
#include "Logger.h"
#include "StringSplit.h"

using namespace std;

namespace marz {

uint16_t real_client_listen_port = 0;
uint16_t real_server_listen_port = 0;
uint16_t real_console_listen_port = 0;

class BalanceSvr : public Svr {
public:
	BalanceSvr() {
		DLOG << "Balance Svr Constructor ......";
	}

	~BalanceSvr() {
		DLOG << "Balance Svr Destructor ......";
	}

	virtual Handler* CreateHandler(const ChannelPtr& channel_ptr) {
		Handler* handler = NULL;
		DLOG << "Create Handler";
        if ((channel_ptr->GetLocalAddress().Port() == real_client_listen_port)) {
            handler = new ClientHandler(channel_ptr);
        } else if ((channel_ptr->GetLocalAddress().Port() == real_server_listen_port)) {
          	handler = new SvrHandler(channel_ptr);
        } else if ((channel_ptr->GetLocalAddress().Port() == real_console_listen_port)) {
        	//handler = new ConsoleHandler();
        }

        return handler;
    }
};

/********** balance svr **********/


} // namespace marz

using namespace marz;

int main(int argc, char* argv[]) {
	signal(SIGPIPE, SIG_IGN);

	Config config("balance_svr.conf");
	
	const char* client_listen_hosts = config.GetValueByName("ClientListenHosts");
	const char* client_listen_port = config.GetValueByName("ClientListenPort");
	const char* server_listen_hosts = config.GetValueByName("ServerListenHosts");
	const char* server_listen_port = config.GetValueByName("ServerListenPort");
	const char* console_listen_hosts = config.GetValueByName("ConsoleListenHosts");
	const char* console_listen_port = config.GetValueByName("ConsoleListenPort");

	if (client_listen_hosts == nullptr || client_listen_port == nullptr 
		|| server_listen_hosts == nullptr || server_listen_port == nullptr 
		|| console_listen_hosts == nullptr || console_listen_port == nullptr) {
		WLOG << "Error : balance_service.conf failed";
		WLOG << client_listen_hosts << " " << client_listen_port 
			<< " " << server_listen_hosts << " " << server_listen_port
			<< " " << console_listen_hosts << " " << console_listen_port;
		return 1;
	} else {
		DLOG << client_listen_hosts << " " << client_listen_port 
			<< " " << server_listen_hosts << " " << server_listen_port
			<< " " << console_listen_hosts << " " << console_listen_port;
	}

	real_client_listen_port = atoi(client_listen_port);
	real_server_listen_port = atoi(server_listen_port);
	real_console_listen_port = atoi(console_listen_port);

	std::vector<IpAddress> listen_hosts;
	std::vector<IpAddress> connect_hosts;

	StringSplit client_listen_host_list(client_listen_hosts, ';');
	for (int i = 0; i < client_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(client_listen_host_list.GetItem(i), real_client_listen_port);
		listen_hosts.push_back(host);
	}

	StringSplit server_listen_host_list(server_listen_hosts, ';');
	for (int i = 0; i < server_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(server_listen_host_list.GetItem(i), real_server_listen_port);
		listen_hosts.push_back(host);
	}

	StringSplit console_listen_host_list(console_listen_hosts, ';');
	for (int i = 0; i < console_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(console_listen_host_list.GetItem(i), real_console_listen_port);
		listen_hosts.push_back(host);
	}

	BalanceSvr* server = new BalanceSvr();

	DLOG << listen_hosts.size() << " " << connect_hosts.size();

	server->SetMainThread(listen_hosts, connect_hosts);
	// server->SetAcceptThread(address_list, accept_thread_cnt);
	server->SetIoThread(2);
	// server->SetConnectThread(address_list) 

	server->Start();
	
	delete server;
	DLOG << "STOP";
	return 0;
}
