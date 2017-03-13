#include <functional>
#include <unistd.h>
#include "EventLoop.h"
#include "Timestamp.h"
#include "Logger.h"
#include "Handle.h"

namespace marz {

EventLoop::EventLoop():
running_(false),
thread_id_(Thread::GetCurrentThreadId()),
selector_(Selector::NewSelector()),
funcs_(),
funcs_running_(false),
heap_(),
read_handle_(NULL),
write_handle_(NULL)
{

}

EventLoop::~EventLoop() {
	if (read_handle_ != NULL) {
        delete read_handle_;
    }
    if (write_handle_ != NULL) {
        delete write_handle_;
    }
}

void EventLoop::Start() {
	if (running_) {
		return;
	}
	running_ = true;

	int fds[2];
	// pipe may have bug
	pipe(fds);
	read_handle_  = new Handle(this, fds[0]);
	write_handle_ = new Handle(this, fds[1]);
	read_handle_->EnableRead();
	read_handle_->SetReadCallback(std::bind(&EventLoop::Consume, this));

	Loop();
}

void EventLoop::Stop() {
	running_ = false;
	if (!IsInSelfThread()) {
		Wakeup();
	}
}

void EventLoop::Loop() {
	int64_t timeout_usec;
	int64_t now;
	int64_t next_timeout;

	while (running_) {
		timeout_usec = 0;
		now = Timestamp::Now().Macrosecond();
		// ? not use lock
		// {
		// 	Lock lock(mutex_);
		// 	if (!heap_->empty()) {
		// 		next_timeout = (heap_->top())->timeout();
		// 		timeout_usec = next_timeout > now ? next_timeout - now : 0; 
		// 	}
		// }
		if (!heap_.Empty()) {
			next_timeout = (heap_.Top())->Timeout();
			timeout_usec = next_timeout > now ? next_timeout - now : 0; 
		}

		selector_->Dispatch(timeout_usec);

		DoTimeout(Timestamp::Now().Macrosecond());
		DoFuncs();
	}
	
}

void EventLoop::DoTimeout(int64_t now) {
	int64_t next_timeout;
	Timer* cur_timer;	
	while (!heap_.Empty()) {
		next_timeout = (heap_.Top())->Timeout();

		if (next_timeout > now) break;
		
		cur_timer = heap_.Pop();
		cur_timer->Run();
		if (cur_timer->IsValid()) {
			DoFunc(std::bind(&EventLoop::PushTimer, this, cur_timer));
		} 
	}
}

void EventLoop::DoFuncs() {
	funcs_running_ = true;

	std::vector<Func> funcs;
	{
		Lock lock(mutex_);

		// std::move()
		funcs_.swap(funcs); 
	}

	for (size_t i = 0; i < funcs.size(); i ++) {
		funcs[i]();
	}

	funcs_running_ = false;
}

TimerId EventLoop::AddTimer(const TimerCallback& cb, int64_t timeout, double interval, int counter) {
	Timer* timer = new Timer(cb, timeout, interval, counter);
	DoFunc(std::bind(&EventLoop::PushTimer, this, timer));
	return TimerId(timer, timer->GetSequence());
}

void EventLoop::RemoveTimer(const TimerId& timerId) {
	Timer* timer = timerId.GetTimer();
	DoFunc(std::bind(&EventLoop::EraseTimer, this, timer));
}

void EventLoop::PushTimer(Timer* timer) {
	heap_.Push(timer);
}

void EventLoop::EraseTimer(Timer* timer) {
	heap_.Erase(timer);
	delete timer;
}

void EventLoop::DoFunc(const Func& func) {
	if (IsInSelfThread()) {
		func();
	} else {
		PushFunc(func);
	}
}

void EventLoop::PushFunc(const Func& func) {
	{
		//TODO: lock free
		Lock lock(mutex_);
		funcs_.push_back(func); 
	}
	if (!IsInSelfThread() || funcs_running_) {
		Wakeup();
	}
}

void EventLoop::Wakeup() {
	int value = 1;
    write(write_handle_->Fd(), &value, sizeof(value));
    DLOG << "Write Wakeup";
}

void EventLoop::Consume() {
    int value;
    // PIPE_BUF is full ? non-atomic. use while loop
    read(read_handle_->Fd(), &value, sizeof(value));
    DLOG << "Read Wakeup";
}

void EventLoop::AddHandle(Handle* handle) { 
	selector_->AddHandle(handle);    
}

void EventLoop::UpdateHandle(Handle* handle) {
	selector_->UpdateHandle(handle); 
}

void EventLoop::RemoveHandle(Handle* handle) { 
	selector_->RemoveHandle(handle); 
}

} // namespace marz

