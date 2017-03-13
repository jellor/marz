#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "Mutex.h"

namespace marz {

class Condition {
public:
	Condition(Mutex& mutex);
	~Condition();

	void Wait();
	bool WaitForSeconds(int seconds);
	void Signal();
	void Broadcast();

private:
	Mutex& mutex_;
	pthread_cond_t condition_;
};

} // namespace marz

#endif // __CONDITION_H__
