#include "SvrMgr.h"
#include "Logger.h"

namespace marz {

SvrMgr::SvrMgr():
total_client_cnt_(0),
server_map_(),
mutex_()
{

}

SvrMgr::~SvrMgr() {

}

int SvrMgr::GetServerCnt() { 
	Lock lock(mutex_);

	return server_map_.size(); 
}

const server_t* SvrMgr::GetServer() {
	uint32_t min_cnt = (uint32_t) -1;

	Lock lock(mutex_);

	std::map<int64_t, server_t*>::iterator min_it = server_map_.end();

	for (auto it = server_map_.begin(); it != server_map_.end(); it ++) {
		server_t* server = it->second;
		if (server->cur_connect_cnt < server->max_connect_cnt 
			&& server->cur_connect_cnt < min_cnt) {
			min_it = it;
			min_cnt = server->cur_connect_cnt;
		}
	}

	if (min_it == server_map_.end()) {
		return NULL;
	} else {
		return min_it->second;
	}
}

void SvrMgr::InsertServer(int64_t key, server_t* server) {
	Lock lock(mutex_);

	server_map_.insert(std::make_pair(key, server));
	total_client_cnt_ += server->cur_connect_cnt;
	DLOG << "server_map_ has inserted in key => " << key;
	DLOG << "server map size => " << server_map_.size();
}

void SvrMgr::DeleteServer(int64_t key) {
	Lock lock(mutex_);

	std::map<int64_t, server_t*>::iterator it = server_map_.find(key);
	if (it != server_map_.end()) {
		delete it->second;
	} else {
		DLOG << "server_map_ has not exist " << key;
	}

	server_map_.erase(key);
	DLOG << "key => " << key;
	DLOG << "msg server info map size => " << server_map_.size();
}

void SvrMgr::Increment(int64_t key, int n) 
{
	Lock lock(mutex_);

	std::map<int64_t, server_t*>::iterator it = server_map_.find(key);
	if (it != server_map_.end()) {
		it->second->cur_connect_cnt += n;
	} else {
		DLOG << "server_map_ has not exist " << key;
	}
}

void SvrMgr::Decrement(int64_t key, int n) 
{
	Lock lock(mutex_);

	std::map<int64_t, server_t*>::iterator it = server_map_.find(key);
	if (it != server_map_.end()) {
		it->second->cur_connect_cnt -= n;
	} else {
		DLOG << "server_map_ has not exist " << key;
	}
}

} // namespace marz
