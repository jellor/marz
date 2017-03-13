#ifndef __SVR_MGR_H__
#define __SVR_MGR_H__

#include <atomic>
#include <map>
#include "NonCopyable.h"
#include "Lock.h"

typedef struct {
	std::string host_name;
	std::string ip_addr_1;
	std::string ip_addr_2;
	uint16_t port;
	
	uint32_t cur_connect_cnt;
	uint32_t max_connect_cnt;
} server_t;

namespace marz {

class SvrMgr : public NonCopyable {
public:
	SvrMgr();
	~SvrMgr();

	int GetServerCnt();

	const server_t* GetServer();

	void InsertServer(int64_t key, server_t* server);
	void DeleteServer(int64_t key);

	void Increment(int64_t key, int n);
	void Decrement(int64_t key, int n);

private:
	uint32_t total_client_cnt_;
	std::map<int64_t, server_t*> server_map_;

	Mutex mutex_;

};

} // namespce marz

#endif // __SVR_MGR_H__