#ifndef __DATABASE_MANAGER_H__
#define __DATABASE_MANAGER_H__

#include <string>
#include <map>
#include "DatabasePool.h"
#include "DatabaseConnector.h"
#include "ResultSet.h"
#include "StringSplit.h"

namespace marz {

class DatabaseManager {
public:
	virtual ~DatabaseManager();

	static DatabaseManager* GetInstance();
	static DatabaseConnector* GetConnector(const char *pool_name) {
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == NULL) {
			return NULL;
		}
		return db_manager->GetDatabaseConnector(pool_name);
	}
	static void ReleaseConnector(DatabaseConnector* database_connector) {
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == NULL) {
			return;
		}
		db_manager->ReleaseDatabaseConnector(database_connector);
	}

	int Init();
	DatabaseConnector* GetDatabaseConnector(const char* pool_name);
	void ReleaseDatabaseConnector(DatabaseConnector* database_connector);

private:
	DatabaseManager();

	static DatabaseManager* instance_;
	std::map<std::string, DatabasePool*> database_pool_map_; 
	
};

} // namespace marz

#endif // __DATABASE_MANAGER_H__
