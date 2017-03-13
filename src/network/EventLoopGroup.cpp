#include "EventLoopGroup.h"

namespace marz {

EventLoopGroup::EventLoopGroup(unsigned int thread_count):
thread_count_(thread_count),
started_(false),
thread_index_(-1),
threads_(NULL)
{
	if (thread_count_ == 0) return;

	threads_ = new SingleThreadEventLoop* [thread_count_];
	if (threads_ == NULL) {
		abort();
	}
	for (int i = 0; i < thread_count_; i ++) {
		threads_[i] = new SingleThreadEventLoop();
	}
}

EventLoopGroup::~EventLoopGroup() {
	if (threads_ == NULL) {
		for (int i = 0; i < thread_count_; i ++) {
			delete threads_[i];
		}
		delete[] threads_;
	} 	
}

void EventLoopGroup::Start() {
	if (started_) return;

	for (int i = 0; i < thread_count_; i ++) {
		threads_[i]->Start();
	}

	started_ = true;
}

bool EventLoopGroup::Join() {
	for (int i = 0; i < thread_count_; i ++) {
		if (!threads_[i]->Join()) return false;
	}
	return true;
}

EventLoop* EventLoopGroup::Next() {
	if (thread_count_ == 0) return NULL;

	thread_index_ = (thread_index_ + 1) % thread_count_;
	return threads_[thread_index_]->GetEventLoop();
}

SingleThreadEventLoop* EventLoopGroup::GetThread(int index) {
	return index >= 0 && index < thread_count_ ? threads_[index] : NULL;
}

} // namespace marz
