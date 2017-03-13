#include "Timer.h"

namespace marz {

std::atomic<uint64_t> Timer::created_count_(0);

Timer::Timer(const TimerCallback& cb, int64_t timeout, double interval, int counter = 1):
timer_callback_(cb),
timeout_(timeout),
interval_(interval),
counter_(counter),
sequence_(GetCreatedCount())
{
	created_count_ ++;
}

Timer::~Timer() {
	
}

bool Timer::IsValid() {
	if (counter_ > 0 || counter_ == -1) {
		timeout_ += static_cast<int64_t>(interval_ * 1000000);
		return true;
	} else {
		return false;
	}
}

bool Timer::operator >(const Timer& rhs) {
	return this->timeout_ > rhs.timeout_;
}

int Timer::GetIndex() const {
	return index_;
}

void Timer::SetIndex(int index) {
	index_ = index;
}

} // namespace marz
