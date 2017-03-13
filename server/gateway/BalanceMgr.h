#ifndef __BALANCE_MGR_H__
#define __BALANCE_MGR_H__

#include <map>
#include "BalanceHandler.h"
#include "Mutex.h"

namespace marz {

class BalanceMgr : public NonCopyable {
public:
	BalanceMgr();
	~BalanceMgr();

	void AddHandler(uint32_t key, BalanceHandler* handler);
	void DelHandler(uint32_t key);

	void NotifyBalanceSvr(uint32_t action, uint32_t user_id);

private:
	std::map<uint32_t, BalanceHandler*> balance_map_;
	mars::Mutex mutex_;

};

} // namespace marz

#endif // __BALANCE_MGR_H__
