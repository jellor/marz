#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>

namespace marz {

class Exception : public std::exception {
public:
	Exception(const char* what) noexcept : what_(what) {}
	Exception(const std::string& what) noexcept : what_(what) {}
	virtual ~Exception() noexcept {}

	virtual const char* What() const noexcept { return what_.c_str(); };
	const std::string ToString() const noexcept;
	const std::string GetStackTrace() const noexcept;

private:
	std::string Demangle(const char* name) const;

	enum { MAX_STACK_TRACE_FRAME_SIZE = 100 };
	std::string what_;

};

} // namespace marz

#endif // __EXCEPTION_H__
