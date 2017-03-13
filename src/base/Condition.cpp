#include <errno.h>
#include <sys/time.h>
#include "Condition.h"


namespace marz {

Condition::Condition(Mutex& mutex):
mutex_(mutex)
{
	pthread_cond_init(&condition_, NULL);
}

Condition::~Condition() {
	pthread_cond_destroy(&condition_);
}

void Condition::Wait() {
	pthread_cond_wait(&condition_, &mutex_.GetMutex());
}

bool Condition::WaitForSeconds(int seconds) {
	struct timespec ts;
	struct timeval  tv;

	gettimeofday(&tv, NULL);

	ts.tv_sec  = tv.tv_sec  + seconds;
	ts.tv_nsec = tv.tv_usec * 1000;

	return pthread_cond_timedwait(&condition_, &mutex_.GetMutex(), &ts) == ETIMEDOUT ? false : true;
}

void Condition::Signal() {
	pthread_cond_signal(&condition_);
}

void Condition::Broadcast() {
	pthread_cond_broadcast(&condition_);
}

} // namespace marz
