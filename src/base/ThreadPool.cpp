#include "ThreadPool.h"
#include "Lock.h"
#include "Logger.h"

namespace marz {

ThreadPool::ThreadPool(unsigned int thread_count):
thread_count_(thread_count),
started_(false),
tasks_(),
mutex_(),
condition_(mutex_),
threads_(nullptr)
{
	threads_ = new Thread* [thread_count_];
}

ThreadPool::~ThreadPool() {
	if (threads_ != NULL) {
		DLOG << "ThreadPool Destructor";
		if (started_) Stop();
		delete[] threads_;
	}
}

void ThreadPool::Start() {
	if (started_) return;
	started_ = true;
	for (int i = 0; i < thread_count_; i ++) {
		char buf[32];
		snprintf(buf, sizeof(buf), "ThreadPool Index -> %d", i);
		threads_[i] = new Thread(std::bind(&ThreadPool::RunInThread, this), buf);
		threads_[i]->Start();
	}
}

void ThreadPool::Stop() {
	{
		Lock lock(mutex_);
		started_ = false;
	}
	condition_.Broadcast();

	for(int i = 0; i < thread_count_; i ++) {
		//threads_[i]->cancel();
		if (threads_[i]->Join()) {
			DLOG << "Join Ok";
		} else {
			DLOG << "Join Fail";
		}
	}
	for (int i = 0; i < thread_count_; i ++) {
		delete threads_[i];
	}
}

void ThreadPool::Push(const Task& task) {
	{
		Lock lock(mutex_);
		tasks_.push(task);
	}
	condition_.Signal();
}

void ThreadPool::RunInThread() {
	Task task;
	while (true) {
		{
			Lock lock(mutex_);
			while (tasks_.empty() && started_) {
				DLOG << "Wait";
				condition_.Wait();
			}
			DLOG << "Before Break";
			if (!started_ && tasks_.empty()) {
                break;
			}
			DLOG << "After  Break";
			task = tasks_.front();
			tasks_.pop();
		}

		try {
			DLOG << "task.star()";
			task.Start();
			DLOG << "task.start() After";
		} catch (...) {
			DLOG << "task.start() Error";
		}

	}
}

} // namespace marz
