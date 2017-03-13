#ifndef __BUFFER_STREAM_H__
#define __BUFFER_STREAM_H__

#include <google/protobuf/io/zero_copy_stream.h>
#include "RingBuffer.h"

namespace marz {

typedef google::protobuf::io::ZeroCopyOutputStream ZeroCopyOutputStream;
typedef google::protobuf::io::ZeroCopyInputStream ZeroCopyInputStream;

class BufferInputStream : public ZeroCopyInputStream {
public:
	BufferInputStream(RingBuffer* buffer);
	~BufferInputStream();

	// implements ZeroCopyInputStream
	virtual bool Next(const void** data, int* size) override;
	virtual void BackUp(int count) override;
	virtual bool Skip(int count) override;
	virtual int64_t ByteCount() const override;

private:
	const RingBuffer* const buffer_;
	const int size_;
	int position_;
	int last_returned_size_;
	int readed_count_;
	
};

class BufferOutputStream : public ZeroCopyOutputStream {
public:
	BufferOutputStream(RingBuffer* buffer);
	~BufferOutputStream();

	// implements ZeroCopyOutputStream
	virtual bool Next(void** data, int* size) override;
	virtual void BackUp(int count) override;
	virtual int64_t ByteCount() const override;

private:
	RingBuffer* const buffer_;
	int last_returned_size_;
	int written_count_;

};

} // namespace marz

#endif // __BUFFER_STREAM_H__
