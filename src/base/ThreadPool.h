#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <queue>
#include "NonCopyable.h"
#include "Thread.h"
#include "Task.h"
#include "Condition.h"

namespace marz {

class ThreadPool : public NonCopyable {
public:
	explicit ThreadPool(unsigned int thread_count);
	~ThreadPool();
	
	void Start();
	void Stop();
	void Push(const Task& task);
	unsigned int Size() const { return thread_count_; }

private:
	void RunInThread();

	const unsigned int thread_count_; 
	bool started_;
	std::queue<Task> tasks_;
	Mutex mutex_;
	Condition condition_;
	Thread** threads_;

};

} // namespace marz

#endif // __THREAD_POOL_H__
