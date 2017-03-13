#include <stdlib.h>
#include <string.h>
#include "SimpleBuffer.h"

namespace marz {

SimpleBuffer::SimpleBuffer()
{
	buffer_ = NULL;

	alloc_size_ = 0;
	write_offset_ = 0;
}

SimpleBuffer::~SimpleBuffer()
{
	alloc_size_ = 0;
	write_offset_ = 0;
	if (buffer_)
	{
		free(buffer_);
		buffer_ = NULL;
	}
}

void SimpleBuffer::Extend(uint32_t len)
{
	alloc_size_ = write_offset_ + len;
	alloc_size_ += alloc_size_ >> 2;	// increase by 1/4 allocate size
	uchar_t* new_buf = (uchar_t*)realloc(buffer_, alloc_size_);
	buffer_ = new_buf;
}

uint32_t SimpleBuffer::Write(void* buf, uint32_t len)
{
	if (write_offset_ + len > alloc_size_)
	{
		Extend(len);
	}

	if (buf)
	{
		memcpy(buffer_ + write_offset_, buf, len);
	}

	write_offset_ += len;

	return len;
}

uint32_t SimpleBuffer::Read(void* buf, uint32_t len)
{
	if (len > write_offset_)
		len = write_offset_;

	if (buf)
		memcpy(buf, buffer_, len);

	write_offset_ -= len;
	memmove(buffer_, buffer_ + len, write_offset_);
	return len;
}

} // namespace marz
