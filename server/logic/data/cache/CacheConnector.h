#ifndef __CACHE_CONNECTOR_H__
#define __CACHE_CONNECTOR_H__

#include <map>
#include <list>
#include <hiredis/hiredis.h>
#include "CachePool.h"

namespace marz {

class CacheConnector {
public:
	CacheConnector(CachePool* pool);
	virtual ~CacheConnector();

	int Init();
	const char* GetPoolName();

	std::string Get(std::string key);
	std::string Setex(std::string key, int timeout, std::string value);
	std::string Set(std::string key, std::string& value);

	bool Mget(const std::vector<std::string>& keys, std::map<std::string, std::string>& ret_values);

	bool IsExists(std::string& key);

	// redis hash structure
	long Hdel(std::string key, std::string field);
	std::string Hget(std::string key, std::string field);
	bool HgetAll(std::string key, std::map<std::string, std::string>& ret_values);
	long Hset(std::string key, std::string field, std::string value);

	long HincrBy(std::string key, std::string field, long value);
	long IncrBy(std::string key, long value);
	std::string Hmset(std::string key, std::map<std::string , std::string>& hash);
	bool Hmget(std::string key, std::list<std::string>& fields, std::list<std::string>& ret_values);

	// atomic operation
	long Incr(std::string key);
	long Decr(std::string key);

	// redis list structure
	long Lpush(std::string key, std::string value);
	long Rpush(std::string key, std::string value);
	long Llen(std::string key);
	bool Lrange(std::string key, long start, long end, std::list<std::string>& ret_values);

private:
	CachePool* cache_pool_;
	redisContext* redis_context_;
	uint64_t last_connect_time_;

};

} // namespace marz

#endif // __CACHE_CONNECTOR_H__
