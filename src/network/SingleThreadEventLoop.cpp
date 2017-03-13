#include <assert.h>
#include "SingleThreadEventLoop.h"

namespace marz {

SingleThreadEventLoop::SingleThreadEventLoop():
started_(false),
thread_(),
event_loop_(NULL)
{

}

SingleThreadEventLoop::~SingleThreadEventLoop() {
	event_loop_->Stop();
	while (started_ == true) {}
	assert(started_ == false);
}

void SingleThreadEventLoop::SetThreadName(const std::string& name) {
	if (started_) {
		return;
	}
	thread_.SetThreadName(name);
}

void SingleThreadEventLoop::Start() {
	thread_.SetThreadFunc(std::bind(&SingleThreadEventLoop::RunInThread, this));
	thread_.Start();
	while (started_ == false) {}
	assert(started_ == true);
}

bool SingleThreadEventLoop::Join() {
	return thread_.Join();
}

EventLoop* SingleThreadEventLoop::GetEventLoop() const {
	return event_loop_;
}

void SingleThreadEventLoop::RunInThread() {

	event_loop_ = new EventLoop();
	started_ = true;
	event_loop_->Start();

	delete event_loop_;
	event_loop_	 = NULL;
	started_ 	 = false;

}

} // namespace marz

