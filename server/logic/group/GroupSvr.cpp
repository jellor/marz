#include <signal.h>
#include "AudioModel.h"
#include "GroupMessageModel.h"
#include "GroupModel.h"
#include "MessageModel.h"
#include "RelationModel.h"
#include "SessionModel.h"
#include "UserModel.h"

#include "Svr.h"
#include "ClientHandler.h"
#include "IpParser.h"
#include "Config.h"
#include "Logger.h"
#include "StringSplit.h"

#include "CacheManager.h"
#include "DatabaseManager.h"

using namespace std;

namespace marz {

uint16_t real_client_listen_port = 0;

class GroupSvr : public Svr {
public:
	GroupSvr() {
		DLOG << "Group Svr Constructor ......";
	}
	~GroupSvr() {
		DLOG << "Group Svr Destructor ......";
	}

	virtual Handler* CreateHandler(const ChannelPtr& channel_ptr) {
		Handler* handler = NULL;

		DLOG << "Create Handler";
        if ((channel_ptr->GetLocalAddress().Port() == real_client_listen_port)) {
            handler = new ClientHandler(channel_ptr);
        } else {
        	DLOG << "Create Handler Failed";
        }

        return handler;
    }
};

} // namespace marz

/********** group server **********/

using namespace marz;

int main(int argc, char* argv[]) {
	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));

	// init business.
	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == NULL) {
		WLOG << "cache_manager is NULL";
		return -1;
	}

	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == NULL) {
		WLOG << "db_manager is NULL";
		return -1;
	}

	if (AudioModel::GetInstance() == NULL) {
		WLOG << "audio model init is failed";
		return -1;
	}

	if (GroupMessageModel::GetInstance() == NULL) {
		WLOG << "group message model init is failed";
		return -1;
	}

	if (GroupModel::GetInstance() == NULL) {
		WLOG << "group model init is failed";
		return -1;
	}

	if (MessageModel::GetInstance() == NULL) {
		WLOG << "message model init is failed";
		return -1;
	}

	if (RelationModel::GetInstance() == NULL) {
		WLOG << "relation model init is failed";
		return -1;
	}

	if (SessionModel::GetInstance() == NULL) {
		WLOG << "session model init is failed";
		return -1;
	}

	if (UserModel::GetInstance() == NULL) {
		WLOG << "user model init is failed";
		return -1;
	}

	Config config("group_svr.conf");
	
	const char* client_listen_hosts = config.GetValueByName("ClientListenHosts");
	const char* client_listen_port = config.GetValueByName("ClientListenPort");

	if (client_listen_hosts == NULL || client_listen_port == NULL) {
		WLOG << "Error : group_svr.conf Failed";
		WLOG << client_listen_hosts << " " << client_listen_port;
		return 1;
	} else {
		DLOG << client_listen_hosts << " " << client_listen_port;
	}

	real_client_listen_port = atoi(client_listen_port);

	std::vector<IpAddress> listen_hosts;
	std::vector<IpAddress> connect_hosts;

	StringSplit client_listen_host_list(client_listen_hosts, ';');
	for (int i = 0; i < client_listen_host_list.GetItemCount(); i ++) {
		IpAddress host(client_listen_host_list.GetItem(i), real_client_listen_port);
		listen_hosts.push_back(host);
	}

	GroupSvr* server = new GroupSvr();

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
