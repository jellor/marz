#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>
#include <string>

namespace marz {

class Timestamp {
public:
	explicit Timestamp(int64_t usec = 0): usec_(usec) {}

	int64_t Macrosecond() const { return usec_; }
	const std::string ToString() const;
	static Timestamp Now();

private:
	int64_t usec_;
};

} // namespace marz

#endif // __TIMESTAMP_H__
