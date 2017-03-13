#ifndef __EVENT_LOOP_GROUP_H__
#define __EVENT_LOOP_GROUP_H__

#include "NonCopyable.h"
#include "EventLoop.h"
#include "SingleThreadEventLoop.h"
#include "IpAddress.h"

namespace marz {

class EventLoopGroup : private NonCopyable {
public:
	EventLoopGroup(unsigned int thread_count = 1);
	~EventLoopGroup();

	void Start();
	bool Join();
	bool IsStarted() const { return started_; }
	EventLoop* Next(); // before should invoke start().  
	SingleThreadEventLoop* GetThread(int index);
	unsigned int Size() const { return thread_count_; }

private:

	const unsigned int thread_count_;
	std::atomic<bool> started_;
	int thread_index_;
	SingleThreadEventLoop** threads_;

};

} // namespace marz

#endif // __EVENT_LOOP_GROUP_H__
