#include "ByteStream.h"

namespace marz {

ByteStream::ByteStream(uchar_t* buf, uint32_t len) {
	buffer_ = buf;
	length_ = len;
	simple_buffer_ = NULL;
	index_ = 0;
}

ByteStream::ByteStream(SimpleBuffer* pSimpBuf, uint32_t pos) {
	simple_buffer_ = pSimpBuf;
	index_ = pos;
	buffer_ = NULL;
	length_ = 0;
}

int16_t ByteStream::ReadInt16(uchar_t *buf) {
	int16_t data = (buf[0] << 8) | buf[1];
	return data;
}

uint16_t ByteStream::ReadUint16(uchar_t* buf) {
	uint16_t data = (buf[0] << 8) | buf[1];
	return data;
}

int32_t ByteStream::ReadInt32(uchar_t *buf) {
	int32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return data;
}

uint32_t ByteStream::ReadUint32(uchar_t *buf) {
	uint32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	return data;
}

void ByteStream::WriteInt16(uchar_t *buf, int16_t data) {
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteUint16(uchar_t *buf, uint16_t data) {
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteInt32(uchar_t *buf, int32_t data) {
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteUint32(uchar_t *buf, uint32_t data) {
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::operator << (int8_t data) {
	WriteByte(&data, 1);
}

void ByteStream::operator << (uint8_t data) {
	WriteByte(&data, 1);
}

void ByteStream::operator << (int16_t data) {
	unsigned char buf[2];
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
	WriteByte(buf, 2);
}

void ByteStream::operator << (uint16_t data) {
	unsigned char buf[2];
	buf[0] = static_cast<uchar_t>(data >> 8);
	buf[1] = static_cast<uchar_t>(data & 0xFF);
	WriteByte(buf, 2);
}

void ByteStream::operator << (int32_t data) {
	unsigned char buf[4];
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
	WriteByte(buf, 4);
}

void ByteStream::operator << (uint32_t data) {
	unsigned char buf[4];
	buf[0] = static_cast<uchar_t>(data >> 24);
	buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
	buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
	buf[3] = static_cast<uchar_t>(data & 0xFF);
	WriteByte(buf, 4);
}

void ByteStream::operator >> (int8_t& data) {
	ReadByte(&data, 1);
}

void ByteStream::operator >> (uint8_t& data) {
	ReadByte(&data, 1);
}

void ByteStream::operator >> (int16_t& data) {
	unsigned char buf[2];

	ReadByte(buf, 2);

	data = (buf[0] << 8) | buf[1];
}

void ByteStream::operator >> (uint16_t& data) {
	unsigned char buf[2];

	ReadByte(buf, 2);

	data = (buf[0] << 8) | buf[1];
}

void ByteStream::operator >> (int32_t& data) {
	unsigned char buf[4];

	ReadByte(buf, 4);

	data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void ByteStream::operator >> (uint32_t& data) {
	unsigned char buf[4];

	ReadByte(buf, 4);

	data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void ByteStream::WriteString(const char *str) {
	uint32_t size = str ? (uint32_t)strlen(str) : 0;

	*this << size;
	WriteByte((void*)str, size);
}

void ByteStream::WriteString(const char *str, uint32_t len) {
	*this << len;
	WriteByte((void*)str, len);
}

char* ByteStream::ReadString(uint32_t& len) {
	*this >> len;
	char* pStr = (char*)GetBuf() + GetPos();
	Skip(len);
	return pStr;
}

void ByteStream::WriteData(uchar_t *data, uint32_t len) {
	*this << len;
	WriteByte(data, len);
}

uchar_t* ByteStream::ReadData(uint32_t &len) {
	*this >> len;
	uchar_t* pData = (uchar_t*)GetBuf() + GetPos();
	Skip(len);
	return pData;
}

void ByteStream::ReadByte(void* buf, uint32_t len) {
	if (index_ + len > length_) {
		//throw Exception("Parase Failed");
	}

	if (simple_buffer_)
		simple_buffer_->Read((char*)buf, len);
	else
		memcpy(buf, buffer_ + index_, len);

	index_ += len;
}

void ByteStream::WriteByte(void* buf, uint32_t len) {
	if (buffer_ && (index_ + len > length_))
		return;

	if (simple_buffer_)
		simple_buffer_->Write((char*)buf, len);
	else
		memcpy(buffer_ + index_, buf, len);

	index_ += len;
}

} // namespace marz
