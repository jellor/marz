#include <sstream>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include "Exception.h"
#include "Logger.h"

namespace marz {

const std::string Exception::ToString() const noexcept {
	std::stringstream ss("");
	if (!what_.empty()) {
		ss << "What: " << what_ << "  |  ";
	}
	ss << "Stack Trace: " << GetStackTrace();
	return ss.str();
}

const std::string Exception::GetStackTrace() const noexcept {
	void* buffer[MAX_STACK_TRACE_FRAME_SIZE];
	int size = backtrace(buffer, MAX_STACK_TRACE_FRAME_SIZE);
	if (size == 0) {
		return "Stack Back Is Empty";
	}
	char** strings = backtrace_symbols(buffer, size);
	if (strings == NULL) {
		return "Backtrace_symbols Return Is NULL";
	}
	std::string result;
	for (int i = 0; i < size; i ++) {
		result.append(Demangle(strings[i]));
		result.push_back('\n');
	}
	free(strings);
	return result;
}

std::string Exception::Demangle(const char* name) const {
	int status;
	std::string demangled(name);
	std::string::size_type begin = demangled.find('(');
	std::string::size_type end   = demangled.find('+', begin);
	DLOG << "Begin " << begin << " End " << end;
	if (begin == std::string::npos || end == std::string::npos) {
		DLOG << "Name Is Parse Error";
		return name;
	}
	char* realName = abi::__cxa_demangle(demangled.substr(begin, end).c_str(), NULL, NULL, &status);
	DLOG << "Status Is " << status;
	if (realName == NULL) {
		DLOG << "ReaL Name Is NULL";
		return name;
	}
	demangled = realName;
	free(realName);
	return demangled;
}

} // namespace marz
