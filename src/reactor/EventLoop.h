#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include <functional>
#include <vector>
#include <memory>
#include "Selector.h"
#include "Lock.h"
#include "Thread.h"
#include "MinHeap.h"
#include "TimerId.h"
#include "NonCopyable.h"

namespace marz {

class Handle;
class Selector;

class EventLoop : public NonCopyable {

	typedef std::function<void(void)> Func;

public:
	EventLoop();
	~EventLoop();

	void Start();
	void Stop();

	bool IsRunning() const { return running_; }
	bool IsInSelfThread() const { return thread_id_ == Thread::GetCurrentThreadId(); }
	void DoFunc(const Func& func);
	TimerId AddTimer(const TimerCallback& cb, int64_t timeout, double interval, int counter = 1);
	void RemoveTimer(const TimerId& timerId);

	void AddHandle(Handle* handle);
	void UpdateHandle(Handle* handle);
    void RemoveHandle(Handle* handle);
  
private:
	void PushTimer(Timer* timer);
	void EraseTimer(Timer* timer);
	void Loop();
	void DoTimeout(int64_t now);
	void DoFuncs();
	void PushFunc(const Func& func);
	void Wakeup();
	void Consume();

	std::atomic<bool> running_;
	const pid_t thread_id_;
	std::unique_ptr<Selector> selector_;
	std::vector<Func> funcs_;
	Mutex mutex_;
	std::atomic<bool> funcs_running_;
	MinHeap<Timer> heap_;

	Handle* read_handle_;
	Handle* write_handle_;

};

} // namespace marz

#endif // __EVENT_LOOP_H__
