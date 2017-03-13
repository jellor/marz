#ifndef __IP_PARSER_H__
#define __IP_PARSER_H__

#include "StringSplit.h"

namespace marz {

class IpParser {
public:
	IpParser() = default;
	virtual ~IpParser() = default;

	inline static bool IsTelcome(const char* ip);

};

bool IpParser::IsTelcome(const char* ip) {
	if (ip == nullptr) {
		return false;
	}

	StringSplit string_split(ip, '.');
	if (string_split.GetItemCount() == 4) {
		return true;
	} else {
		return false;
	}
}

} // namespace marz

#endif // __IP_PARSER_H__
