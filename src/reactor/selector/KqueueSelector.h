#ifndef __KQUEUE_SELECTOR_H__
#define __KQUEUE_SELECTOR_H__

#ifdef __APPLE__

#include <set>
#include <vector>
#include <sys/event.h>
#include "../Selector.h"
#include "../Handle.h"

namespace marz {

class KqueueSelector : public Selector {
public:
	KqueueSelector();
	~KqueueSelector();

	void AddHandle(Handle* handle)      override;
	void UpdateHandle(Handle* handle)   override;
	void RemoveHandle(Handle* handle)   override;
	void Dispatch(int64_t timeout_usec) override;

private:
	const int kqfd_;
	std::set<Handle*> handle_set_;
	std::vector<struct kevent> active_event_list_;
	
};

} // namespace marz

#endif // __APPLE__
#endif // __KQUEUE_SELECTOR_H__
