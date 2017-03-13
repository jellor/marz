#include <errno.h>
#include <sys/time.h>
#include "Logger.h"
#include "Thread.h"

namespace marz {

static const char* LOGGER_LEVEL_STRING[] = {
	"TRACE ",
	"DEDUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

void DefaultLoggerWriteCb(const char* data, size_t size) {
	fwrite(data, 1, size, stdout);
}

/***************** Class LoggerImpl *****************/
LoggerImpl::LoggerImpl(LOGGER_LEVEL level, const char* file, const char* func, int line): 
logger_stream_() 
{
	struct timeval tv;
	struct tm tm;
	char buf[32];
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);
	snprintf(buf, sizeof(buf), 
		"%4d-%02d-%02d %02d:%02d:%02d.%06d ", 
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec);
	logger_stream_ << buf;
	const char* file_name = file; //strrchr(file, '/');
	if (file_name == NULL) {
		file_name = file;
	} else {
		file_name += 1;
	}
	logger_stream_ << Thread::GetCurrentThreadName() << ' ' << Thread::GetCurrentThreadId() << ' ';
	logger_stream_ << file_name << ' ' << func << ' ' << line << ' ' << LOGGER_LEVEL_STRING[level];
}

/******************  Class Logger  ******************/
LOGGER_LEVEL Logger::logger_level_  = DEFAULT_LOGGER_LEVEL;
const char* Logger::path_   	    = DEFAULT_LOGGER_PATH;
LoggerWriteCallback Logger::logger_write_cb_ = DefaultLoggerWriteCb;

Logger::Logger(LOGGER_LEVEL level, const char* file, const char* func, int line):
logger_impl_(level, file, func, line),
enable_(Logger::GetLoggerLevel() <= level)
{}

Logger::~Logger() {
	GetLoggerStream() << '\n';
	std::string str = GetLoggerStream().ToString();
	logger_write_cb_(str.c_str(), str.size());
}

std::string Logger::GetError() {
	char buf[256];
	strerror_r(errno, buf, sizeof(buf));
	return std::string(buf);
}

} // namespace marz
