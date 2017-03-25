#include <signal.h>
#include "Svr.h"
#include "GatewayHandler.h"
#include "GroupHandler.h"
#include "LoginHandler.h"
#include "MsgHandler.h"
#include "RelationHandler.h"
#include "SessionHandler.h"
#include "UserHandler.h"
#include "IpParser.h"
#include "Config.h"
#include "Logger.h"
#include "StringSplit.h"

using namespace std;

namespace marz {


std::string g_client_listen_host; // = NULL;

uint16_t real_client_listen_port = 0;
uint16_t real_console_listen_port = 0;

uint16_t real_group_port = 0;
uint16_t real_login_port = 0;
uint16_t real_message_port = 0;
uint16_t real_relation_port = 0;
uint16_t real_session_port = 0;
uint16_t real_user_port = 0;


class RouteSvr : public Svr {
public:
	RouteSvr() {
		DLOG << "Balance Service Constructor ......";
	}

	~RouteSvr() {
		DLOG << "Balance Service Destructor ......";
	}

	virtual Handler* CreateHandler(const ChannelPtr& channel_ptr) override {

		Handler* handler = NULL;

        if ((channel_ptr->GetLocalAddress().Port() == real_client_listen_port)) {
            handler = new GatewayHandler(channel_ptr);
        } else if ((channel_ptr->GetLocalAddress().Port() == real_console_listen_port)) {
        	// handler = new ConsoleHandler();
        } else if ((channel_ptr->GetPeerAddress().Port() == real_group_port)) {
            handler = new GroupHandler(channel_ptr);
        } else if ((channel_ptr->GetPeerAddress().Port() == real_login_port)) {
            handler = new LoginHandler(channel_ptr);
        } else if ((channel_ptr->GetPeerAddress().Port() == real_message_port)) {
            handler = new MsgHandler(channel_ptr);
        } else if ((channel_ptr->GetPeerAddress().Port() == real_relation_port)) {
            handler = new RelationHandler(channel_ptr);
        } else if ((channel_ptr->GetPeerAddress().Port() == real_session_port)) {
            handler = new SessionHandler(channel_ptr);
        } else if ((channel_ptr->GetPeerAddress().Port() == real_user_port)) {
            handler = new UserHandler(channel_ptr);
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

	Config config("route_svr.conf");
	
	const char* client_listen_hosts = config.GetValueByName("ClientListenHosts");
	const char* client_listen_port = config.GetValueByName("ClientListenPort");

	const char* console_listen_hosts = config.GetValueByName("ConsoleListenHosts");
	const char* console_listen_port = config.GetValueByName("ConsoleListenPort");

	const char* group_hosts = config.GetValueByName("GroupHosts");
	const char* group_port = config.GetValueByName("GroupPort");

	const char* login_hosts = config.GetValueByName("LoginHosts");
	const char* login_port = config.GetValueByName("LoginPort");

	const char* message_hosts = config.GetValueByName("MessageHosts");
	const char* message_port = config.GetValueByName("MessagePort");

	const char* relation_hosts = config.GetValueByName("RelationHosts");
	const char* relation_port = config.GetValueByName("RelationPort");

	const char* session_hosts = config.GetValueByName("SessionHosts");
	const char* session_port = config.GetValueByName("SessionPort");

	const char* user_hosts = config.GetValueByName("UserHosts");
	const char* user_port = config.GetValueByName("UserPort");

	if (client_listen_hosts == NULL || client_listen_port == NULL 
		|| console_listen_hosts == NULL || console_listen_port == NULL
		|| group_hosts == NULL || group_port == NULL 
		|| login_hosts == NULL || login_port == NULL || message_hosts == NULL || message_port == NULL) {
		WLOG << "Error : gateway_svr.conf Failed";
		WLOG << client_listen_hosts << " " << client_listen_port 
			<< " " << console_listen_hosts << " " << console_listen_port
			<< " " << group_hosts << " " << group_port
			<< " " << login_hosts << " " << login_port;
		return 1;
	} else {
		DLOG << client_listen_hosts << " " << client_listen_port 
			<< " " << console_listen_hosts << " " << console_listen_port
			<< " " << group_hosts << " " << group_port
			<< " " << login_hosts << " " << login_port;
	}

	real_client_listen_port = atoi(client_listen_port);
	real_console_listen_port = atoi(console_listen_port);

	real_group_port = atoi(group_port);
	real_login_port = atoi(login_port);
	real_message_port = atoi(message_port);
	real_relation_port = atoi(relation_port);
	real_session_port = atoi(session_port);
	real_user_port = atoi(user_port);

	std::vector<IpAddress> listen_hosts;
	std::vector<IpAddress> connect_hosts;

	StringSplit client_listen_host_list(client_listen_hosts, ';');
	for (int i = 0; i < client_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(client_listen_host_list.GetItem(i), real_client_listen_port);
		listen_hosts.push_back(host);
	}

	StringSplit console_listen_host_list(console_listen_hosts, ';');
	for (int i = 0; i < console_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(console_listen_host_list.GetItem(i), real_console_listen_port);
		listen_hosts.push_back(host);
	}

	StringSplit group_host_list(group_hosts, ';');
	for (int i = 0; i < group_host_list.GetItemCount(); i ++) {
		IpAddress host(group_host_list.GetItem(i), real_group_port);
		connect_hosts.push_back(host);
	}

	StringSplit login_host_list(login_hosts, ';');
	for (int i = 0; i < login_host_list.GetItemCount(); i ++) {
		IpAddress host(login_host_list.GetItem(i), real_login_port);
		connect_hosts.push_back(host);
	}

	StringSplit message_host_list(message_hosts, ';');
	for (int i = 0; i < message_host_list.GetItemCount(); i ++) {
		IpAddress host(message_host_list.GetItem(i), real_message_port);
		connect_hosts.push_back(host);
	}

	StringSplit relation_host_list(relation_hosts, ';');
	for (int i = 0; i < relation_host_list.GetItemCount(); i ++) {
		IpAddress host(relation_host_list.GetItem(i), real_relation_port);
		connect_hosts.push_back(host);
	}

	StringSplit session_host_list(session_hosts, ';');
	for (int i = 0; i < session_host_list.GetItemCount(); i ++) {
		IpAddress host(session_host_list.GetItem(i), real_session_port);
		connect_hosts.push_back(host);
	}

	StringSplit user_host_list(user_hosts, ';');
	for (int i = 0; i < user_host_list.GetItemCount(); i ++) {
		IpAddress host(user_host_list.GetItem(i), real_user_port);
		connect_hosts.push_back(host);
	}

	RouteSvr* server = new RouteSvr();

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
