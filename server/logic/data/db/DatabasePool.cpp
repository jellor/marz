#include "DatabasePool.h"
#include "DatabaseConnector.h"
#include "Lock.h"

namespace marz {

DatabasePool::DatabasePool(const char* pool_name, const char* database_server_ip, uint16_t database_server_port,
		const char* user_name, const char* password, const char* database_name, int max_connect_count):
pool_name_(pool_name),
database_server_ip_(database_server_ip),
database_server_port_(database_server_port),
user_name_(user_name),
password_(password),
database_name_(database_name),
max_connect_count_(max_connect_count),
mutex_(),
condition_(mutex_)
{
	current_connect_count_ = 2;
}

DatabasePool::~DatabasePool() {
	for (std::list<DatabaseConnector*>::iterator it = free_list_.begin(); it != free_list_.end(); it ++) {
		DatabaseConnector* connector = *it;
		delete connector;
	}

	free_list_.clear();
}

int DatabasePool::Init() {

	for (int i = 0; i < current_connect_count_; i ++) {
		DatabaseConnector* connector = new DatabaseConnector(this);
		int ret = connector->Init();
		if (ret != 0) {
			delete connector;
			return ret;
		}
		free_list_.push_back(connector);
	}

	return 0;
}

DatabaseConnector* DatabasePool::GetDatabaseConnector() {
	
	Lock lock(mutex_);

	while (free_list_.empty()) {
		if (current_connect_count_ >= max_connect_count_) {
			condition_.Wait();
		} else {
			DatabaseConnector* connector = new DatabaseConnector(this);
			int ret = connector->Init();
			if (ret != 0) {
				delete connector;
				return NULL;
			} else {
				free_list_.push_back(connector);
				current_connect_count_ ++;
			}
		}
	}

	DatabaseConnector* connector = free_list_.front();
	free_list_.pop_front();


	return connector;
}

void DatabasePool::ReleaseDatabaseConnector(DatabaseConnector* database_connector) {
	
	Lock lock(mutex_);

	std::list<DatabaseConnector*>::iterator it = free_list_.begin();
	for (; it != free_list_.end(); it ++) {
		if (*it == database_connector) {
			break;
		}
	}

	if (it == free_list_.end()) {
		free_list_.push_back(database_connector);
	}

	condition_.Signal();
}

} // namespce marz
