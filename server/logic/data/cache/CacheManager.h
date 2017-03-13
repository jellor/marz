#ifndef __CACHE_MANAGER_H__
#define __CACHE_MANAGER_H__

#include <map>
#include <string>
#include "CachePool.h"
#include "CacheConnector.h"

namespace marz {

class CacheManager {
public:
	virtual ~CacheManager();

	static CacheManager* GetInstance();
	static CacheManager* GetInstance(const char *config_path);
	static CacheConnector *GetConnector(const char *pool_name) {
		CacheManager* cache_manager = CacheManager::GetInstance();
		if (cache_manager == NULL) {
			return NULL;
		}
		return cache_manager->GetCacheConnector(pool_name);
	}
	static void ReleaseConnector(CacheConnector* connector) {
		CacheManager* cache_manager = CacheManager::GetInstance();
		if (cache_manager == NULL) {
			return;
		}
		cache_manager->ReleaseCacheConnector(connector);
	}

	int Init();
	int Init(const char *config_path);
	CacheConnector* GetCacheConnector(const char* pool_name);
	void ReleaseCacheConnector(CacheConnector* connector);

private:
	CacheManager();

	static CacheManager* instance_;
	std::map<std::string, CachePool*> cache_pool_map_;

};

} // namespace marz

#endif // __CACHE_MANAGER_H__
