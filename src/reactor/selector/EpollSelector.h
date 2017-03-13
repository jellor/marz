#ifndef __EPOLL_SELECTOR_H__
#define __EPOLL_SELECTOR_H__

#ifdef __linux__

#include <set>
#include <vector>
#include <sys/epoll.h>
#include "Selector.h"

namespace marz {

class EpollSelector : public Selector {
public:
	EpollSelector();
	~EpollSelector();
	
	void AddHandle(Handle* handle)      override;
	void UpdateHandle(Handle* handle)   override;
	void RemoveHandle(Handle* handle)   override;
	void Dispatch(int64_t timeout_usec) override;

private:
	const int epfd_;
	std::set<Handle*> handle_set_;
	std::vector<struct epoll_event> active_event_list_;

};

} // namespace marz

#endif // __linux__
#endif // __EPOLL_SELECTOR_H__
