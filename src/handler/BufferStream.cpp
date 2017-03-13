#include "BufferStream.h"

namespace marz {

BufferInputStream::BufferInputStream(RingBuffer* buffer):
buffer_(buffer),
size_(buffer_->Size()),
position_(buffer_->HeadIndex()),
last_returned_size_(0),
readed_count_(0)
{

}

BufferInputStream::~BufferInputStream() {

}

bool BufferInputStream::Next(const void** data, int* size) {
	if (buffer_->Empty()) return false;
	if (readed_count_ < size_) {
		if (buffer_->HeadIndex() > buffer_->TailIndex()) {
			*data = (buffer_->Base() + position_);
			last_returned_size_ = size_ - readed_count_;
		} else {
			if (position_ < buffer_->Capacity()) {
				*data = (buffer_->Base() + position_);
				last_returned_size_ = buffer_->Capacity() - position_;
			} else {
				*data = (buffer_->Base() + position_);
				last_returned_size_ = buffer_->TailIndex();
			}
		}
		*size = last_returned_size_;
		position_ = (position_ + last_returned_size_) % buffer_->Capacity();
		readed_count_ += last_returned_size_;
		return true;
	} else {
		// we're at the end of the buffer.
		last_returned_size_ = 0; // Don't let caller back up.
		return false;
	}
}

void BufferInputStream::BackUp(int count) {
	position_ = (position_ + buffer_->Capacity() - count) % buffer_->Capacity();
	last_returned_size_ = 0; // Dont' let caller back up further.
	readed_count_ -= count;
}

bool BufferInputStream::Skip(int count) {
	last_returned_size_ = 0; // Don't let caller back up.
	if (count > size_ - readed_count_) {
		position_ = buffer_->TailIndex();
		readed_count_ = size_;
		return false;
	} else {
		position_ = (position_ + count) % buffer_->Capacity();
		readed_count_ += count;
		return true;
	}
}

int64_t BufferInputStream::ByteCount() const {
	return readed_count_;
}

BufferOutputStream::BufferOutputStream(RingBuffer* buffer):
buffer_(buffer),
last_returned_size_(0),
written_count_(0)
{

}

BufferOutputStream::~BufferOutputStream() {

}

bool BufferOutputStream::Next(void** data, int* size) {
	if (buffer_->Unused() == 0) {
		buffer_->Extend(1024);
	}
	if (buffer_->HeadIndex() <= buffer_->TailIndex()) {
		*data = const_cast<char*>(buffer_->Tail());
		last_returned_size_ = buffer_->Capacity() - buffer_->TailIndex();
	} else {
		*data = const_cast<char*>(buffer_->Tail());
		last_returned_size_ = buffer_->HeadIndex() - buffer_->TailIndex();
	}
	*size = last_returned_size_;
	written_count_ += last_returned_size_;
	buffer_->SkipFromTail(last_returned_size_);
	return true;
}

void BufferOutputStream::BackUp(int count) {
	buffer_->BackUpFromTail(count);
	written_count_ -= count;
}

int64_t BufferOutputStream::ByteCount() const {
	return written_count_;
}

} // namespace marz
