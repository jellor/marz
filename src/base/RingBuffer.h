#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <string.h>
#include <string>
#include "NonCopyable.h"

namespace marz {

class RingBuffer : public NonCopyable {
public:
	RingBuffer();
	RingBuffer(unsigned int size);
	~RingBuffer();

	int Read(int sockfd);
	int Write(int sockfd);
	int ReadBytes(char* data, unsigned int size);
	int Write(const char* data, unsigned int size);
	int Write(const std::string& str);
	bool Empty() const { return size_ == 0; }
	const char* Base() const { return buffer_; }
	const char* Head() const { return buffer_ + head_index; }
	const char* Tail() const { return buffer_ + tail_index; }
	unsigned int HeadIndex() const { return head_index; }
	unsigned int TailIndex() const { return tail_index; }
	unsigned int Size() const { return size_; }
	unsigned int Capacity() const { return capacity_; }
	unsigned int Unused() const { return capacity_ - size_; }
	void Resize(unsigned int size);
	void Extend(unsigned int size);

	bool BackUpFromHead(unsigned int count);
	bool SkipFromHead(unsigned int count);
	bool BackUpFromTail(unsigned int count);
	bool SkipFromTail(unsigned int count);

	void Adjust();
	void Clear();

	int16_t GetInt16();
	int32_t GetInt32();
	
	int16_t ReadInt16();
	int32_t ReadInt32();
	
private:
	unsigned int head_index; // ? signed int head_index
	unsigned int tail_index;
	unsigned int size_;
	unsigned int capacity_;
	char* buffer_;

	static const unsigned int kInitSize = 256;
	
};

} // namespace marz

#endif // __RING_BUFFER_H__
