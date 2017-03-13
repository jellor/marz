#ifndef __ATTACH_H__
#define __ATTACH_H__

#include "SimpleBuffer.h"

namespace marz {

enum {
	ATTACH_TYPE_HANDLE	= 1,
    ATTACH_TYPE_PDU = 2,
    ATTACH_TYPE_HANDLE_AND_PDU = 3,
    ATTACH_TYPE_PDU_FOR_PUSH = 4,
    ATTACH_TYPE_HANDLE_AND_PDU_FOR_FILE = 5,
};

class DatabaseAttach {
public:
	DatabaseAttach(uint32_t type, uint32_t handle, uint32_t service_type = 0);				// 序列化
	DatabaseAttach(uchar_t* attach_data, uint32_t attach_len);	// 反序列化
	virtual ~DatabaseAttach() {}

	uchar_t* GetBuffer() {return simple_buffer_.GetBuffer(); }
	uint32_t GetLength() { return simple_buffer_.GetWriteOffset(); }
	uint32_t GetType() { return type_; }
	uint32_t GetHandle() { return handle_; }
    uint32_t GetServiceType() { return service_type_; }
private:
	SimpleBuffer	simple_buffer_;
	uint32_t 		type_;
	uint32_t		handle_;
    uint32_t        service_type_;
};

class PacketAttach {
public:
	PacketAttach(uint32_t type, uint32_t handle, uint32_t pduLength, uchar_t* pdu, uint32_t service_type = 0);				// 序列化
	PacketAttach(uchar_t* attach_data, uint32_t attach_len);	// 反序列化
	virtual ~PacketAttach() {}
    
	uchar_t* GetBuffer() {return simple_buffer_.GetBuffer(); }
	uint32_t GetLength() { return simple_buffer_.GetWriteOffset(); }
	uint32_t GetType() { return type_; }
	uint32_t GetHandle() { return handle_; }
    uint32_t GetServiceType() { return service_type_; }
    uint32_t GetPduLength() { return length_; }
    uchar_t* GetPdu() { return buffer_; }
private:
	SimpleBuffer	simple_buffer_;
	uint32_t 		type_;
	uint32_t		handle_;
    uint32_t        service_type_;
    uint32_t        length_;
    uchar_t*        buffer_;
};

} // namespace marz

#endif // __ATTACH_H__
