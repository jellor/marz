#include <stdio.h>
#include <sys/syscall.h> 
#include <unistd.h>
#include <errno.h>
#include "Thread.h"
#include "Exception.h"
#include "Logger.h"

namespace marz {

__thread pid_t thread_tid = 0;
__thread const char* thread_name = NULL;


void AfterFork() {
	thread_tid  = 0;
	Thread::GetCurrentThreadId();
	thread_name = "Main";
}

class ThreadInitialer {
public:
	ThreadInitialer() {
		thread_tid = 0;
		thread_tid = Thread::GetCurrentThreadId();
		thread_name = "Main";	
		pthread_atfork(NULL, NULL, AfterFork);
		DLOG << "ThreadInitialer Successful";
	}
};
ThreadInitialer threadInitialer;


/*************** Internal Class ThreadSpace ***************
 *
 *   描 述 ：主要是用于线程内的执行
 *
 *********************************************************/
class ThreadSpace {
public:
	ThreadSpace(const ThreadFunc& thread_func, const std::string& name, const std::shared_ptr<pid_t>& tid);
	~ThreadSpace();

	void StartThread();
	void Release();

private:
	std::weak_ptr<pid_t> tid_;
	std::string name_;
	ThreadFunc thread_func_;
};

ThreadSpace::ThreadSpace(const ThreadFunc& thread_func, const std::string& name, const std::shared_ptr<pid_t>& tid):
thread_func_(thread_func),
name_(name),
tid_(tid)
{}

ThreadSpace::~ThreadSpace() {
}

// 在新创建线程中执行
void ThreadSpace::StartThread() {
	std::shared_ptr<pid_t> shared_tid = tid_.lock();
	if (shared_tid != NULL) {
		*shared_tid = static_cast<pid_t>(Thread::GetCurrentThreadId());
		shared_tid.reset();
	}

	// strcpy()
	thread_name = name_.empty() ? "Unknown Thread" : name_.c_str();

#ifdef __linux__
	pthread_setname_np(pthread_self(), name_.c_str());
#elif __APPLE__
	pthread_setname_np(name_.c_str());
#else
#error "Platform Not Supported"
#endif

	try {
		thread_func_();
	} catch (const Exception& e) {

	} catch (const std::exception& e) {

	} catch (...) {

	} 
	
}
/*********************************************************/

// 线程的入口函数
void* ThreadRoutine(void* arg) {
	ThreadSpace* thread = static_cast<ThreadSpace*>(arg);
	thread->StartThread();
	delete thread;
	return NULL;
}

std::atomic<unsigned int> Thread::created_count_(0);

Thread::Thread(const ThreadFunc& thread_func, const std::string& name):
started_(false),
joined_(false),
name_(name),
tid_(new pid_t(0)),
thread_id_(0),
thread_func_(thread_func)
{	
	created_count_++;
	if (name_.empty()) {
		char buf[32];
		snprintf(buf, sizeof(buf), "Unknown Thread %d", created_count_.load());
		name_ = buf;
	}
}

Thread::~Thread() {
	if (started_ && !joined_) {
		printf("Detach");
		pthread_detach(thread_id_);
		// int flag = pthread_kill(thread_id_, 0);  
		// if(flag == ESRCH) {
		// 		"指定的线程不存在或者是已经终止" 
		// } else if(flag == EINVAL) {
		// 		"调用传递一个无用的信号"
		// } else {
		// 		"线程存在"
		// } 
	}
}

void Thread::SetThreadFunc(const ThreadFunc& thread_func) {
	thread_func_ = thread_func;
} 

void Thread::SetThreadName(const std::string& name) {
	name_ = name;
}

bool Thread::Start() {
	if (thread_func_ == nullptr) return false;
	started_ = true;
	ThreadSpace* arg = new ThreadSpace(thread_func_, name_, tid_);
	int ret = pthread_create(&thread_id_, NULL, ThreadRoutine, static_cast<void*>(arg));
	if (ret == 0) {
		return true;
	} else {
		started_ = false;
		delete arg;
		return false;
	}
}

bool Thread::Join() {
	int ret = -1;
	if (started_ && !joined_) {
		joined_  = true;
		ret = pthread_join(thread_id_, NULL);
		started_ = false;	
	}
	return ret == 0 ? true : false;
}

bool Thread::Cancel() {
	int ret = -1;
	if (thread_id_ != 0) {
		ret = pthread_cancel(thread_id_);
		started_ = false;
	}
	return ret == 0 ? true : false;
}

pid_t Thread::GetCurrentThreadId() {
	if (thread_tid == 0) {

#ifdef __linux__
		thread_tid = static_cast<pid_t>(syscall(SYS_gettid));
#elif __APPLE__
		//can use pthread_getthreadid_np() or pthread_threadid_np().
		static pid_t apple_tid = 1;
		thread_tid = apple_tid ++;
#else
#error "Platform Not Supported"
#endif
	
		if (errno) {
			printf("Happened errno In Thread::getCurrentThreadId %s\n", strerror(errno));
		}
	}
	return thread_tid;
}

std::string Thread::GetCurrentThreadName() {
	if (thread_name == NULL) {
		thread_name = "Unknown Thread";
	}
	return thread_name;
}

} // namespace marz
