#include <sys/time.h>
#include "Timestamp.h"

namespace marz {

Timestamp Timestamp::Now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return Timestamp(tv.tv_sec * 1000 * 1000 + tv.tv_usec);
}

const std::string Timestamp::ToString() const {
	struct tm tm;
	time_t sec = usec_ / 1000000;
	char buf[32];
	localtime_r(&sec, &tm);
	snprintf(buf, sizeof(buf), 
		"%4d-%02d-%02d %02d:%02d:%02d.%06d ", 
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
		tm.tm_min, tm.tm_sec, static_cast<suseconds_t>(usec_ % 1000000));
	return buf;
}

} // namespace marz
