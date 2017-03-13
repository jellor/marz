#ifndef __BASE64_H__
#define __BASE64_H__

#include <string>

namespace marz {

class Base64 {
public:
	static std::string Encode(const std::string& binary_data);
	static std::string Decode(const std::string& ascii_data);

private:
	static const char encoding_table[64];
	static const char decoding_table[128];

};

} // namespace marz

#endif // __BASE64_H__
