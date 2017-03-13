#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <pthread.h>

namespace marz {

template <class T>
class Singleton {
public:
	static T& GetInstance() {
		pthread_once(&thread_once_, &Singleton::Init);

		return *instance_;
	}

private:
	Singleton() = default;
	virtual ~Singleton() = default;

	static void Init() {
		instance_ = new T();
	}

	static T* instance_;
	static pthread_once_t thread_once_;
};

template <class T>
T* Singleton<T>::instance_ = nullptr;

template <class T>
pthread_once_t Singleton<T>::thread_once_ = PTHREAD_ONCE_INIT;

} // namespace marz

#endif // __SINGLETON_H__
