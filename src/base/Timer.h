#ifndef __TIMER_H__
#define __TIMER_H__

#include <functional>
#include <atomic>
#include "MinHeap.h"

namespace marz {

typedef std::function<void(void)> TimerCallback;

class Timer : public MinHeapInterface<Timer>, public NonCopyable {	
public:
	explicit Timer(const TimerCallback& cb, int64_t timeout, double interval, int counter);
	~Timer();

	void SetTimerCallback(const TimerCallback& cb) { timer_callback_ = cb; }
	void Run() { timer_callback_(); if (counter_ != -1 ) counter_ --; }
	bool IsValid();
	int64_t Timeout() const { return timeout_; }

	// MinHeapInterface implement
	bool operator >(const Timer& rhs) override;
	int  GetIndex() const override;
	void SetIndex(int index) override;
	const uint64_t GetSequence() const { return sequence_; }

	static uint64_t GetCreatedCount() { return created_count_.load(); }

private:
	TimerCallback timer_callback_; 
	int64_t timeout_;
	const double interval_;
	int counter_; // -1 => forever, 0 => remove immediately, x > 0 => will trigger x timeout. 
	int index_;
	const uint64_t sequence_;

	static std::atomic<uint64_t> created_count_;

};

} // namespace marz

#endif // __TIMER_H__
