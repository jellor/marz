#ifndef __LOG_H__
#define __LOG_H__

#include <string.h>
#include <string>
//#include <functional>
#include "NonCopyable.h"

namespace marz {

typedef enum {
		LOGGER_TRACE = 0,
		LOGGER_DEBUG = 1,
		LOGGER_INFO  = 2,
		LOGGER_WARN  = 3,
		LOGGER_ERROR = 4,
		LOGGER_FATAL = 5,
} LOGGER_LEVEL;

static const LOGGER_LEVEL   DEFAULT_LOGGER_LEVEL = LOGGER_DEBUG;
static const char*          DEFAULT_LOGGER_PATH  = "./log";

/***************** Class LoggerStream *****************/
class LoggerStream : public NonCopyable {
public:
	LoggerStream(): buffer_() {}
	~LoggerStream() {}

    inline LoggerStream& operator <<(const void* val);
	inline LoggerStream& operator <<(const char* val);
	inline LoggerStream& operator <<(bool val);
	inline LoggerStream& operator <<(char val);
	inline LoggerStream& operator <<(unsigned char val);
	inline LoggerStream& operator <<(short val);
	inline LoggerStream& operator <<(unsigned short val);
	inline LoggerStream& operator <<(int val);
	inline LoggerStream& operator <<(unsigned int val);
	inline LoggerStream& operator <<(long val);
	inline LoggerStream& operator <<(unsigned long val);
	inline LoggerStream& operator <<(long long val);
	inline LoggerStream& operator <<(unsigned long long val);
	inline LoggerStream& operator <<(float val);
	inline LoggerStream& operator <<(double val);
	inline LoggerStream& operator <<(const std::string& val);

	inline std::string ToString() { return buffer_.ToString(); }
	
private:
	template<class T>
	LoggerStream& Write(const char* fmt, T t);

	// Internal Class
	class Buffer {
	public:
		Buffer(): cur_index_(0) { memset(buffer_, 0, sizeof(buffer_));}
		void Reset()  { cur_index_ = 0; memset(buffer_, 0, sizeof(buffer_)); }
		char* Begin() { return buffer_ + cur_index_;   }
		char* End()   { return buffer_ + sizeof(buffer_); }
		bool Available() const { return cur_index_ < MAX_BUFFER_SIZE; }	
		int Length()     const { return MAX_BUFFER_SIZE - cur_index_; }
		void Add(int length)   { cur_index_ += length; }
		std::string ToString() { return std::string(buffer_); }

		static const int MAX_BUFFER_SIZE = 4 * 1024;
	private:
		char buffer_[MAX_BUFFER_SIZE]; // ? heap object
		int cur_index_; 
	};
	Buffer buffer_;
};

inline LoggerStream& LoggerStream::operator <<(const void* val) {
	return Write("0X%p", val);
}

inline LoggerStream& LoggerStream::operator <<(const char* val) {
	return Write("%s", val);
}

inline LoggerStream& LoggerStream::operator <<(bool val) {
	return val ? Write("%s", "true") : Write("%s", "false");
}

inline LoggerStream& LoggerStream::operator <<(char val) {
	return Write("%c", val);
}

inline LoggerStream& LoggerStream::operator <<(unsigned char val) {
	return Write("%c", val);
}

inline LoggerStream& LoggerStream::operator <<(short val) {
	return Write("%d", static_cast<int>(val));
}

inline LoggerStream& LoggerStream::operator <<(unsigned short val) {
	return Write("%u", static_cast<unsigned short>(val));
}

inline LoggerStream& LoggerStream::operator <<(int val) {
	return Write("%d", val);
}

inline LoggerStream& LoggerStream::operator <<(unsigned int val) {
	return Write("%u", val);
}

inline LoggerStream& LoggerStream::operator <<(long val) {
	return Write("%ld", val);
}

inline LoggerStream& LoggerStream::operator <<(unsigned long val) {
	return Write("%lu", val);
}

inline LoggerStream& LoggerStream::operator <<(long long val) {
	return Write("%lld", val);
}

inline LoggerStream& LoggerStream::operator <<(unsigned long long val) {
	return Write("%llu", val);
}

inline LoggerStream& LoggerStream::operator <<(float val) {
	return Write("%.6f", val);
}

inline LoggerStream& LoggerStream::operator <<(double val) {
	return Write("%.6lf", val);
}

inline LoggerStream& LoggerStream::operator <<(const std::string& val) {
	return Write("%s", val.c_str());
}

template<class T>
inline LoggerStream& LoggerStream::Write(const char* fmt, T t) {
	if (buffer_.Available()) {
		int len = snprintf(buffer_.Begin(), buffer_.Length(), fmt, t);
		if (len < 0) {
			buffer_.Reset();
		} else if (len >= buffer_.Length()) {
			*(buffer_.End() - 1) = '\0';
			buffer_.Add(len);
		} else {
			buffer_.Add(len);
		}
	}
	return *this;
}

/***************** Class LogImpl *****************/
class LoggerImpl {
public:
	LoggerImpl(LOGGER_LEVEL level, const char* file, const char* func, int line);
	~LoggerImpl() {}

	LoggerStream& GetLoggerStream() { return logger_stream_; }

private:
	LoggerStream logger_stream_;
};

/******************  Class Logger  ******************/
//typedef std::function<void (const char*, size_t size)> LogWriteCallback;
typedef void (*LoggerWriteCallback)(const char*, size_t size);

class Logger {
public:
	Logger(LOGGER_LEVEL level, const char* file, const char* func, int line);
	~Logger();

	bool Enable() const { return enable_; }
	LoggerStream& GetLoggerStream() { return logger_impl_.GetLoggerStream(); }

	static LOGGER_LEVEL GetLoggerLevel() { return logger_level_; }
	static void SetLoggerLevel(LOGGER_LEVEL logger_level) { logger_level_ = logger_level; }
	static const char* GetLoggerPath() { return path_; }
	static void SetLoggerPath(const char* path) { path_ = path; }

	static void SetLoggerWriteCallback(const LoggerWriteCallback& cb) { logger_write_cb_ = cb; }
	static std::string GetError();

private:
	LoggerImpl logger_impl_;
	bool enable_;
	static LOGGER_LEVEL logger_level_;
	static const char* path_;
	static LoggerWriteCallback logger_write_cb_;
};

/******************  Logger Macro  ******************/
#define TLOG Logger(LOGGER_TRACE, __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()
#define DLOG Logger(LOGGER_DEBUG, __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()
#define ILOG Logger(LOGGER_INFO,  __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()
#define WLOG Logger(LOGGER_WARN,  __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()
#define ELOG Logger(LOGGER_ERROR, __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()
#define FLOG Logger(LOGGER_FATAL, __FILE__, __FUNCTION__, __LINE__).GetLoggerStream()

} // namespace marz

#endif // __LOGGER_H__
