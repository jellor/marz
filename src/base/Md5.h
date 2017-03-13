#ifndef __MD5_H__
#define __MD5_H__

namespace marz {

class Md5 {
public:
	static void Calculate(const char *content, unsigned int len, char *md5);
};

} // namespace marz

#endif // __MD5_H__
