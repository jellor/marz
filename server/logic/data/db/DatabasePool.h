#ifndef __DATABASE_POOL_H__
#define __DATABASE_POOL_H__

#include <string>
#include <list>
#include "Condition.h"

namespace marz {

class DatabaseConnector;

class DatabasePool {
public:
	DatabasePool(const char* pool_name, const char* database_server_ip, uint16_t database_server_port,
		const char* user_name, const char* password, const char* database_name, int max_connect_count);
	virtual ~DatabasePool();

	int Init();
	DatabaseConnector* GetDatabaseConnector();
	void ReleaseDatabaseConnector(DatabaseConnector* database_connector);

	const char* GetPoolName() { return pool_name_.c_str(); }
	const char* GetDatabaseServerIp() { return database_server_ip_.c_str(); }
	uint16_t GetDatabaseServerPort() { return database_server_port_; }
	const char* GetDatabaseUserName() { return user_name_.c_str(); }
	const char* GetDatabasePassword() { return password_.c_str(); }
	const char* GetDatabaseName() { return database_name_.c_str(); }

private:
	std::string pool_name_;
	std::string database_server_ip_;
	uint16_t database_server_port_;
	std::string user_name_;
	std::string password_;
	std::string database_name_;
	int current_connect_count_;
	int max_connect_count_;

	std::list<DatabaseConnector*> free_list_;
	Mutex mutex_;
	Condition condition_;

};

} // namespace marz

#endif // __DATABASE_POOL_H__
