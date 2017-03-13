#ifndef __THREAD_H__
#define __THREAD_H__

#include <atomic>
#include <functional>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <memory>
#include "NonCopyable.h"

namespace marz {

typedef std::function<void(void)> ThreadFunc;

class Thread : public NonCopyable {
public:
	Thread(const ThreadFunc& thread_func = nullptr, const std::string& name = std::string());
	~Thread();

	void SetThreadFunc(const ThreadFunc& thread_func);
	void SetThreadName(const std::string& name);
	bool Start();
	bool Join();
	bool Cancel();
	bool IsStarted() const { return started_; }
	int  GetId() const { return *tid_; }

	static void Sleep(double seconds) { usleep(seconds * 1000000); }
	static bool IsMainThread() { return strcmp(GetCurrentThreadName().c_str(), "Main") == 0 ? true : false; }
	static int GetCreatedCount() { return created_count_.load(); }
	static int GetCurrentThreadId();
	static std::string GetCurrentThreadName();

private:
	std::atomic<bool> started_;
	std::atomic<bool> joined_;
	std::string name_;
	std::shared_ptr<pid_t> tid_;
	pthread_t thread_id_;
	ThreadFunc thread_func_;

	static std::atomic<unsigned int> created_count_;
};

} // namespace marz

#endif // __THREAD_H__
