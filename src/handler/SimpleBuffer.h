#ifndef __SIMPLE_BUFFER_H__
#define __SIMPLE_BUFFER_H__

#include <set>
#include <map>
#include <list>
#include <string>
#include "Types.h"

namespace marz {

class SimpleBuffer {
public:
	SimpleBuffer();
	~SimpleBuffer();
	uchar_t*  GetBuffer() { return buffer_; }
	uint32_t GetAllocSize() { return alloc_size_; }
	uint32_t GetWriteOffset() { return write_offset_; }
	void IncWriteOffset(uint32_t len) { write_offset_ += len; }

	void Extend(uint32_t len);
	uint32_t Write(void* buf, uint32_t len);
	uint32_t Read(void* buf, uint32_t len);
private:
	uchar_t*	buffer_;
	uint32_t	alloc_size_;
	uint32_t	write_offset_;
};

} // namespace marz

#endif // __SIMPLE_BUFFER_H__