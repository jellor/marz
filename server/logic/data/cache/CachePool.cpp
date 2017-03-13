#include "CachePool.h"
#include "Lock.h"
#include "CacheConnector.h"

namespace marz {

CachePool::CachePool(const char* pool_name, const char* server_ip, uint16_t server_port, int db_number, int max_connect_count):
pool_name_(pool_name),
server_ip_(server_ip),
server_port_(server_port),
db_number_(db_number),
max_connect_count_(max_connect_count),
mutex_(),
condition_(mutex_)
{
	current_connect_count_ = 2;
}

CachePool::~CachePool() {
	Lock lock(mutex_);
	for (std::list<CacheConnector*>::iterator it = free_list_.begin(); it != free_list_.end(); it ++) {
		CacheConnector* connector = *it;
		delete connector;
	}

	free_list_.clear();
	current_connect_count_ = 0;
}

int CachePool::Init() {
	for (size_t i = 0; i < current_connect_count_; i ++) {
		CacheConnector* connector = new CacheConnector(this);
		if (connector == NULL || connector->Init() != 0) {
			delete connector;
			return 1;
		}

		free_list_.push_back(connector);
	}

	return 0;
}

CacheConnector* CachePool::GetCacheConnector() {
	Lock lock(mutex_);

	while (free_list_.empty()) {
		if (current_connect_count_ >= max_connect_count_) {
			condition_.Wait();
		} else {
			CacheConnector* connector = new CacheConnector(this);
			if (connector == NULL || connector->Init() != 0) {
				delete connector;
				return nullptr;
			} else {
				free_list_.push_back(connector);
				current_connect_count_ ++;
			}
		}
	}

	CacheConnector* connector = free_list_.front();
	free_list_.pop_front();
	return connector;
}

void CachePool::ReleaseCacheConnector(CacheConnector* connector) {
	Lock lock(mutex_);

	std::list<CacheConnector*>::iterator it = free_list_.begin();
	for (; it != free_list_.end(); it ++) {
		if (*it == connector) {
			break;
		}
	}

	if (it == free_list_.end()) {
		free_list_.push_back(connector);
	}
	condition_.Signal();
}

} // namespace marz
