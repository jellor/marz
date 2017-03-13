#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>
#include "NonCopyable.h"
#include "Exception.h"

namespace marz {

class Mutex : public NonCopyable {
public:
	Mutex() {
		pthread_mutexattr_t mutexattr;
		pthread_mutexattr_init(&mutexattr);
		pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE); // should consider about pthread_cond_t since it should PTHREAD_MUTEX_NAMED_NP.
		if (pthread_mutex_init(&mutex_, &mutexattr)) {
			pthread_mutexattr_destroy(&mutexattr);
			throw Exception("Cannot Create Mutex");
		}
		pthread_mutexattr_destroy(&mutexattr);
	}
	~Mutex() { pthread_mutex_destroy(&mutex_); }

	inline void Lock();
	inline void Unlock();

	pthread_mutex_t& GetMutex() { return mutex_; }

private:
	pthread_mutex_t mutex_;
};

inline void Mutex::Lock() {
	if (0 != pthread_mutex_lock(&mutex_)) {
		throw Exception("Cannot Lock Mutex");
	}
}

inline void Mutex::Unlock() {
	if (0 != pthread_mutex_unlock(&mutex_)) {
		throw Exception("Cannot Unlock Mutex");
	}
}

} // namespace marz

#endif //
