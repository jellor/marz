#ifndef __LOCK_H__
#define __LOCK_H__

#include <pthread.h>
#include "NonCopyable.h"
#include "Exception.h"
#include "Mutex.h"

namespace marz {

class Lock : public NonCopyable {
public:
	explicit Lock(Mutex& mutex): mutex_(mutex) { mutex_.Lock(); }
	~Lock() { mutex_.Unlock(); }

private:
	Mutex& mutex_;

};

} // marz

#endif // __LOCK_H__
