#ifndef __CACHE_POOL_H__
#define __CACHE_POOL_H__

#include <vector>
#include <list>
#include "Mutex.h"
#include "Condition.h"

namespace marz {

class CacheConnector;

class CachePool {
public:
	CachePool(const char* pool_name, const char* server_ip, uint16_t server_port, int db_number, int max_connect_count);
	virtual ~CachePool();

	int Init();

	CacheConnector* GetCacheConnector();
	void ReleaseCacheConnector(CacheConnector* connector);

	const char* GetPoolName() const { return pool_name_.c_str(); }
	const char* GetServerIp() const { return server_ip_.c_str(); }
	uint16_t GetServerPort() const { return server_port_; }
	int GetDbNumber() const { return db_number_; }

private:
	std::string pool_name_;
	std::string server_ip_;
	uint16_t server_port_;
	int db_number_;
	int current_connect_count_;
	int max_connect_count_;


	std::list<CacheConnector*> free_list_;
	Mutex mutex_;
	Condition condition_;

};

} // namespace marz

#endif // __CACHE_POOL_H__
