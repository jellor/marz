#ifndef __SINGLE_THREAD_EVENT_LOOP_H__
#define __SINGLE_THREAD_EVENT_LOOP_H__

#include "Thread.h"
#include "EventLoop.h"

namespace marz {

class SingleThreadEventLoop {
public:
	SingleThreadEventLoop();
	~SingleThreadEventLoop();

	void Start();
	bool Join();
	EventLoop* GetEventLoop() const;
	bool IsStarted() const { return event_loop_ != NULL; }
	void SetThreadName(const std::string& name);

private:
	void RunInThread();

	std::atomic<bool> started_;
	Thread thread_;	
	EventLoop* event_loop_;
	
};

} // namespace marz

#endif // __SINGLE_THREAD_EVENT_LOOP_H__
