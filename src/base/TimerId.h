#ifndef __TIMER_ID_H__
#define __TIMER_ID_H__

#include "Timer.h"

namespace marz {

class TimerId {
public:
	TimerId(Timer* timer, int64_t sequence): timer_(timer), sequence_(sequence) {}
	Timer* GetTimer() const { return timer_; }

private:
	Timer* const timer_;
	const uint64_t sequence_;

};

} // namespace marz

#endif // __TIMER_ID_H__
