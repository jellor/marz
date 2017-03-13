#ifndef __BYTE_STREAM_H__
#define __BYTE_STREAM_H__

#include "SimpleBuffer.h"

namespace marz {

class ByteStream {
public:
	ByteStream(uchar_t* buf, uint32_t len);
	ByteStream(SimpleBuffer* pSimpBuf, uint32_t pos);
	~ByteStream() {}

	unsigned char* GetBuf() { return simple_buffer_ ? simple_buffer_->GetBuffer() : buffer_; }
	uint32_t GetPos() { return index_; }
	uint32_t GetLen() { return length_; }
	void Skip(uint32_t len) {
		index_ += len;
		if(index_ > length_) {
			//throw Exception("Parse Failed");
		}
	}

	static int16_t ReadInt16(uchar_t* buf);
	static uint16_t ReadUint16(uchar_t* buf);
	static int32_t ReadInt32(uchar_t* buf);
	static uint32_t ReadUint32(uchar_t* buf);
	static void WriteInt16(uchar_t* buf, int16_t data);
	static void WriteUint16(uchar_t* buf, uint16_t data);
	static void WriteInt32(uchar_t* buf, int32_t data);
	static void WriteUint32(uchar_t* buf, uint32_t data);

	void operator << (int8_t data);
	void operator << (uint8_t data);
	void operator << (int16_t data);
	void operator << (uint16_t data);
	void operator << (int32_t data);
	void operator << (uint32_t data);

	void operator >> (int8_t& data);
	void operator >> (uint8_t& data);
	void operator >> (int16_t& data);
	void operator >> (uint16_t& data);
	void operator >> (int32_t& data);
	void operator >> (uint32_t& data);

	void WriteString(const char* str);
	void WriteString(const char* str, uint32_t len);
	char* ReadString(uint32_t& len);

	void WriteData(uchar_t* data, uint32_t len);
	uchar_t* ReadData(uint32_t& len);

private:
	void WriteByte(void* buf, uint32_t len);
	void ReadByte(void* buf, uint32_t len);

	SimpleBuffer*	simple_buffer_;
	uchar_t*		buffer_;
	uint32_t		length_;
	uint32_t		index_;
	
};

} // namespace marz

#endif // __BYTE_STREAM_H__