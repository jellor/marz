#include "Attach.h"
#include "ByteStream.h"

namespace marz {

DatabaseAttach::DatabaseAttach(uint32_t type, uint32_t handle, uint32_t service_type) {
	ByteStream os(&simple_buffer_, 0);

	os << type;
	os << handle;
    os << service_type;
    std::wstring wstrIn;
}

DatabaseAttach::DatabaseAttach(uchar_t* attach_data, uint32_t attach_len) {
	ByteStream is(attach_data, attach_len);

	is >> type_;
	is >> handle_;
    is >> service_type_;
}

PacketAttach::PacketAttach(uint32_t type, uint32_t handle, uint32_t pduLength, uchar_t* pdu, uint32_t service_type) {
    ByteStream os(&simple_buffer_, 0);
    
	os << type;
	os << handle;
    os << service_type;
    os.WriteData(pdu, pduLength);
}

PacketAttach::PacketAttach(uchar_t* attach_data, uint32_t attach_len) {
    ByteStream is(attach_data, attach_len);
    
	is >> type_;
	is >> handle_;
    is >> service_type_;
    buffer_ = is.ReadData(length_);
}

} // namespace marz
