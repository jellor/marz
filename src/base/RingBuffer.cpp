#include <stdlib.h>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "RingBuffer.h"
#include "Logger.h"

namespace marz {

RingBuffer::RingBuffer():
head_index(0),
tail_index(0),
size_(0)
{
	buffer_ = static_cast<char*>(malloc(kInitSize));
	if (buffer_ == NULL) {

	}
	capacity_ = kInitSize;
}

RingBuffer::RingBuffer(unsigned int size):
head_index(0),
tail_index(0),
size_(0)
{
	buffer_ = static_cast<char*>(malloc(size));
	if (buffer_ == NULL) {}
	capacity_ = size;
}

RingBuffer::~RingBuffer() {
	DLOG << "RingBuffer Destructor ...";
	if (buffer_ != NULL) {
		free(buffer_);
	}
}

int RingBuffer::Read(int sockfd) {
	if (Unused() == 0) Resize(0);
	int size;
	if (head_index <= tail_index) {
		struct iovec iv[2];
		iv[0].iov_base = buffer_ + tail_index;
		iv[0].iov_len  = capacity_ - tail_index;
		iv[1].iov_base = buffer_;
		iv[1].iov_len  = head_index;
		size = ::readv(sockfd, iv, 2);
	} else {
		size = ::read(sockfd, buffer_ + tail_index, head_index - tail_index);
	}
	if (size < 0) {
		DLOG << "--------- IMPORTANT ERROR ----------";
		DLOG << "ERRNO => " << errno;
		DLOG << "--------- IMPORTANT ERROR ----------";
	}
	tail_index = (tail_index + size) % capacity_;
	size_      = size_ + size;
	return size;
}

int RingBuffer::Write(int sockfd) {
	if (size_ == 0) return 0;
	int size;
	if (head_index >= tail_index) {
		struct iovec iv[2];
		iv[0].iov_base = buffer_ + head_index;
		iv[0].iov_len  = capacity_ - head_index;
		iv[1].iov_base = buffer_;
		iv[1].iov_len  = tail_index;
		size = ::writev(sockfd, iv, 2);
	} else {
		size = ::write(sockfd, buffer_ + head_index, tail_index - head_index);
	}
	if (size < 0) {
		DLOG << "--------- IMPORTANT ERROR ----------";
		DLOG << "ERRNO => " << errno;
		DLOG << "--------- IMPORTANT ERROR ----------";
	}
	head_index = (head_index + size) % capacity_;
	size_      = size_ - size;
	return size;
}

int RingBuffer::ReadBytes(char* data, unsigned int size) {
	if (size_ == 0) return 0;
	unsigned int resultSize = size > this->size_ ? this->size_ : size;
	if (head_index >= tail_index) {
		unsigned int len = capacity_ - head_index;
		memcpy(data, buffer_ + head_index, len);
		memcpy(data + len, buffer_, resultSize - len);
	} else {
		memcpy(data, buffer_ + head_index, resultSize);
	}
	head_index = (head_index + resultSize) % capacity_; // ?
	size_ 	   = size_ - resultSize;
	return resultSize;
}

int RingBuffer::Write(const char* data, unsigned int size) {
	if (Unused() < size) {
		Resize(capacity_ + size);
	}
	assert(Unused() >= size);

	unsigned int len = capacity_ - tail_index;
	if (size > len) {
		memcpy(buffer_ + tail_index, data, len);
		memcpy(buffer_, data + len, size - len);
	} else {
		memcpy(buffer_ + tail_index, data, size);
	}
	tail_index = (tail_index + size) % capacity_;
	size_      = size_ + size;
	return size;
}

int RingBuffer::Write(const std::string& str) {
    return Write(str.c_str(), str.size());
}

void RingBuffer::Resize(unsigned int size) {
	int tmp_size = capacity_ << 1; // ? tempSize != capacity_
	int new_size = tmp_size > size ? tmp_size : size;
	buffer_ = static_cast<char*>(realloc(buffer_, new_size));
	if (buffer_ == NULL) {

	} else {
		if (head_index  == tail_index && size_ == 0) {
			head_index = 0;
			tail_index = 0;
		} else if (head_index >= tail_index) {
			if (tail_index != 0) { // ? can delete the statement.
				memcpy(buffer_ + capacity_, buffer_, tail_index);
			}
			tail_index += capacity_;
		}
		capacity_ = new_size;
	}
}

void RingBuffer::Extend(unsigned int size) {
	int new_size = capacity_ + size;
	buffer_ = static_cast<char*>(realloc(buffer_, new_size));
	if (buffer_ == NULL) {

	} else {
		if (head_index == tail_index && size_ == 0) {
			head_index = 0;
			tail_index = 0;
		} else if (head_index >= tail_index) {
			if (tail_index != 0) { // ? can delete the statement.
				memcpy(buffer_ + capacity_, buffer_, tail_index);
			}
			tail_index += capacity_;
		}
		capacity_ = new_size;
	}
}

bool RingBuffer::BackUpFromHead(unsigned int count) {
	if (size_ + count > capacity_) return false;
	head_index = (head_index + capacity_ - count) % capacity_;
	size_	   = size_ + count;
}

bool RingBuffer::SkipFromHead(unsigned int count) {
	if (size_ < count) return false;
	head_index = (head_index + count) % capacity_;
	size_      = size_ - count;
}

bool RingBuffer::BackUpFromTail(unsigned int count) {
	if (size_ < count) return false;
	tail_index = (tail_index + capacity_ - count) % capacity_;
	size_      = size_ - count;
	return true;
}

bool RingBuffer::SkipFromTail(unsigned int count) {
	if (size_ + count > capacity_) return false;
	tail_index = (tail_index + count) % capacity_;
	size_      = size_ + count;
}

void RingBuffer::Adjust() {
	if (head_index >= tail_index && size_ != 0) {
		char* tmp = static_cast<char*>(malloc(tail_index));
		memcpy(tmp, buffer_, tail_index);
		memcpy(buffer_, buffer_ + head_index, capacity_ - head_index);
		memcpy(buffer_ + (capacity_ - head_index), tmp, tail_index);
		head_index = 0;
		tail_index = size_;
		free(tmp);
	}
	assert(head_index <= tail_index);
}

void RingBuffer::Clear() {
	head_index = 0;
	tail_index = 0;
	size_	   = 0;
}

int16_t RingBuffer::GetInt16() {
	Adjust();
    int16_t retVal = *((int16_t*)(buffer_ + head_index));
    return retVal;
}

int32_t RingBuffer::GetInt32() {
	Adjust();
	int32_t retVal = *((int32_t*)(buffer_ + head_index));
	return retVal;
}

int16_t RingBuffer::ReadInt16() {
	Adjust();
	int16_t retVal = *((int16_t*)(buffer_ + head_index));
	head_index += 2;
	size_ -= 2;
	return retVal;
}

int32_t RingBuffer::ReadInt32() {
	Adjust();
	int32_t retVal = *((int32_t*)(buffer_ + head_index));
	head_index += 4;
	size_ -= 4;
	return retVal;
}

} // namespace marz
