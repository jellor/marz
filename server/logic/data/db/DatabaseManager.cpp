#include <stdlib.h>
#include "DatabaseManager.h"
#include "DatabaseConnector.h"
#include "Config.h"
#include "Logger.h"

namespace marz {

DatabaseManager* DatabaseManager::instance_ = NULL;

DatabaseManager::DatabaseManager():
database_pool_map_()
{
}

DatabaseManager::~DatabaseManager() {
}

DatabaseManager* DatabaseManager::GetInstance() {
	if (instance_ == NULL) {
		instance_ = new DatabaseManager();
		if (instance_->Init() != 0) {
			delete instance_;
			instance_ = NULL;
		}
	}

	return instance_;
}

int DatabaseManager::Init() {
	Config config("database.conf");

	const char* database_instances = config.GetValueByName("DatabaseInstances");
	if (database_instances == NULL) {
		WLOG << "Not Configure DatabaseInstances";
		return 1;
	}

	char host[64];
	char port[64];
	char database_name[64];
	char user_name[64];
	char user_password[64];
	char max_connect[64];

	StringSplit string_split(database_instances, ',');
	for (int i = 0; i < string_split.GetItemCount(); i ++) {
		char* name = string_split.GetItem(i);
		snprintf(host, 64, "%s_host", name);
		snprintf(port, 64, "%s_port", name);
		snprintf(database_name, 64, "%s_database_name", name);
		snprintf(user_name, 64, "%s_user_name", name);
		snprintf(user_password, 64, "%s_user_password", name);
		snprintf(max_connect, 64, "%s_max_connect", name);

		const char* db_host = config.GetValueByName(host);
		const char* db_port = config.GetValueByName(port);
		const char* db_database_name = config.GetValueByName(database_name);
		const char* db_user_name = config.GetValueByName(user_name);
		const char* db_user_password = config.GetValueByName(user_password);
		const char* db_max_connect = config.GetValueByName(max_connect);

		if (db_host == nullptr || db_port == nullptr || db_database_name == nullptr 
			|| db_user_name == nullptr || db_user_password == nullptr || db_max_connect == nullptr) {
			WLOG << "Configure For Database Is Failed";
			return 2;
		}

		int real_db_port = atoi(db_port);
		int real_max_connect = atoi(db_max_connect);
		DatabasePool* database_pool = new DatabasePool(name, db_host, real_db_port, db_user_name, 
			db_user_password, db_database_name, real_max_connect);
		if (database_pool == nullptr) {
			WLOG << "DatabasePool Constructor Failed";
			return 3;
		}

		if (database_pool->Init() != 0) {
			WLOG << "DatabasePool Init Failed";
			return 4;
		}

		database_pool_map_.insert(std::make_pair(name, database_pool));
	}

	return 0;
}

DatabaseConnector* DatabaseManager::GetDatabaseConnector(const char* pool_name) {
	std::map<std::string, DatabasePool*>::iterator it = database_pool_map_.find(pool_name);
	if (it == database_pool_map_.end()) {
		return nullptr;
	} else {
		return it->second->GetDatabaseConnector();
	}
}	

void DatabaseManager::ReleaseDatabaseConnector(DatabaseConnector* database_connector) {
	if (database_connector == nullptr) {
		return;
	}

	std::map<std::string, DatabasePool*>::iterator it = database_pool_map_.find(database_connector->GetPoolName());
	if (it == database_pool_map_.end()) {
		return;
	} else {
		return it->second->ReleaseDatabaseConnector(database_connector);
	}

}

} // namespace marz
