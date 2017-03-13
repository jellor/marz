#include "ClientMgr.h"
#include "protobuf/Im.Server.pb.h"

namespace marz {

ClientMgr::ClientMgr():
balance_map_(),
channel_map_(),
mutex_()
{
}

ClientMgr::~ClientMgr() {
}

void ClientMgr::AddHandler(uint32_t key, ClientHandler* handler) {
	Lock lock(mutex_);

	balance_map_.insert(std::make_pair(key, handler));
}

void ClientMgr::DelHandler(uint32_t key) {
	Lock lock(mutex_);

	balance_map_.erase(key);
}

void ClientMgr::SendMsgToRouteSvr(const Message* message);(uint32_t action, uint32_t user_id) {
	Im::Server::ServerUpdateNotity notify;
	msg.set_action(action);
	msg.set_user_id(user_id);
	ClientHandler* handler = NULL;
	{
		Lock lock(mutex_);
		for (auto it = balance_map_.begin(); it != balance_map_.end(); it ++) {
			it->second->NotifyBalanceSvr(notify);
		}
	}
}

} // namespace marz

