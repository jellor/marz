#include <stdlib.h>
#include "CacheManager.h"
#include "CacheConnector.h"
#include "Config.h"
#include "StringSplit.h"
#include "Logger.h"

namespace marz {

CacheManager* CacheManager::instance_ = NULL;

CacheManager::CacheManager():
cache_pool_map_()
{
}

CacheManager::~CacheManager()
{
}

CacheManager* CacheManager::GetInstance() {
	if (instance_ == NULL) {
		instance_ = new CacheManager();
		if (instance_->Init() != 0) {
			delete instance_;
			instance_ = NULL;
		}
	}

	return instance_;
}

CacheManager* CacheManager::GetInstance(const char* config_path) {
	if (instance_ == NULL) {
		instance_ = new CacheManager();
		if (instance_->Init(config_path) != 0) {
			delete instance_;
			instance_ = NULL;
		}
	}

	return instance_;
}

int CacheManager::Init(const char* config_path) {
	Config config(config_path);

	const char* cache_instances = config.GetValueByName("CacheInstances");
	if (cache_instances == NULL) {
		WLOG << "Not Configure CacheInstances";
		return 1;
	}

	char host[64];
	char port[64];
	char db_number[64];
	char max_connect[64];

	StringSplit string_split(cache_instances, ',');
	for (int i = 0; i < string_split.GetItemCount(); i ++) {
		char* name = string_split.GetItem(i);
		snprintf(host, 64, "%s_host", name);
		snprintf(port, 64, "%s_port", name);
		snprintf(db_number, 64, "%s_db_number", name);
		snprintf(max_connect, 64, "%s_max_connect", name);

		const char* cache_host = config.GetValueByName(host);
		const char* cache_port = config.GetValueByName(port);
		const char* cache_db_number = config.GetValueByName(db_number);
		const char* cache_max_connect = config.GetValueByName(max_connect);

		if (cache_host == NULL || cache_port == NULL 
			|| cache_db_number == NULL || cache_max_connect == NULL) {
			WLOG << "Configure For Cache Is Failed";
			return 2;
		}

		int real_cache_port = atoi(cache_port);
		int real_cache_db_number = atoi(cache_db_number);
		int real_cache_max_connect = atoi(cache_max_connect);
		CachePool* cache_pool = new CachePool(name, cache_host, real_cache_port, 
			real_cache_db_number, real_cache_max_connect);
		if (cache_pool == NULL) {
			WLOG << "CachePool Constructor Failed";
			return 3;
		}

		if (cache_pool->Init() != 0) {
			WLOG << "CachePool Init Failed";
			return 4;
		}

		cache_pool_map_.insert(std::make_pair(name, cache_pool));
	}

	return 0;
}

int CacheManager::Init() {
	Config config("cache.conf");

	const char* cache_instances = config.GetValueByName("CacheInstances");
	if (cache_instances == NULL) {
		WLOG << "Not Configure CacheInstances";
		return 1;
	}

	char host[64];
	char port[64];
	char db_number[64];
	char max_connect[64];

	StringSplit string_split(cache_instances, ',');
	for (int i = 0; i < string_split.GetItemCount(); i ++) {
		char* name = string_split.GetItem(i);
		snprintf(host, 64, "%s_host", name);
		snprintf(port, 64, "%s_port", name);
		snprintf(db_number, 64, "%s_db_number", name);
		snprintf(max_connect, 64, "%s_max_connect", name);

		const char* cache_host = config.GetValueByName(host);
		const char* cache_port = config.GetValueByName(port);
		const char* cache_db_number = config.GetValueByName(db_number);
		const char* cache_max_connect = config.GetValueByName(max_connect);

		if (cache_host == NULL || cache_port == NULL 
			|| cache_db_number == NULL || cache_max_connect == NULL) {
			WLOG << "Configure For Cache Is Failed";
			return 2;
		}

		int real_cache_port = atoi(cache_port);
		int real_cache_db_number = atoi(cache_db_number);
		int real_cache_max_connect = atoi(cache_max_connect);
		CachePool* cache_pool = new CachePool(name, cache_host, real_cache_port, 
			real_cache_db_number, real_cache_max_connect);
		if (cache_pool == NULL) {
			WLOG << "CachePool Constructor Failed";
			return 3;
		}

		if (cache_pool->Init() != 0) {
			WLOG << "CachePool Init Failed";
			return 4;
		}

		cache_pool_map_.insert(std::make_pair(name, cache_pool));
	}

	return 0;
}

CacheConnector* CacheManager::GetCacheConnector(const char* pool_name) {
	std::map<std::string, CachePool*>::iterator it = cache_pool_map_.find(pool_name);
	if (it != cache_pool_map_.end()) {
		return it->second->GetCacheConnector();
	} else {
		return NULL;
	}
}

void CacheManager::ReleaseCacheConnector(CacheConnector* connector) {
	if (connector == NULL) {
		return;
	}

	std::map<std::string, CachePool*>::iterator it = cache_pool_map_.find(connector->GetPoolName());
	if (it != cache_pool_map_.end()) {
		it->second->ReleaseCacheConnector(connector);
	}
}

} // namespce marz
