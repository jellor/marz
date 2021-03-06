#include <string>
#include <unistd.h>
#include <google/protobuf/message.h>
#include "PacketHandler.h"
#include "BufferStream.h"
#include "Logger.h"
#include "Util.h"

namespace marz {

// typedef struct Packet {
//     int32_t length;
//     int16_t version;
//     int16_t service;
//     int16_t command;
//     int16_t sequence;
//     int16_t reserve;
//     char*   data;
//     int32_t check_sum;
// } Packet;

const int HEADER_LEN = 20;

void PacketHandler::OnReceive(const ChannelPtr& channel_ptr) {
        DLOG << "Handle Read";
        while (Valid(channel_ptr->GetMutableInBuffer())) {
            Packet* packet = Decode(channel_ptr->GetMutableInBuffer());
            if(!packet) {
                DLOG << "Cannot Decode Message";
                break;
            }
            PRINT_PACKET(packet);
            OnMessage(channel_ptr, packet);
        }
    }

bool PacketHandler::Valid(RingBuffer* buffer) {
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    int buffer_size = buffer->Size();
    if (buffer_size >= HEADER_LEN && buffer->GetInt16() <= buffer_size && buffer->GetInt16() > 0) {
        return true;
    } else {
        return false;
    }
}

void PacketHandler::Encode(Packet* packet, RingBuffer** buf) {
    int data_size = packet->length;

    RingBuffer* buffer = new RingBuffer(packet->length);

    buffer->Write(reinterpret_cast<const char*>(&packet->length),   2);
    buffer->Write(reinterpret_cast<const char*>(&packet->version),  2);
    buffer->Write(reinterpret_cast<const char*>(&packet->ip),  4);
    buffer->Write(reinterpret_cast<const char*>(&packet->port),  2);
    buffer->Write(reinterpret_cast<const char*>(&packet->service),  2);
    buffer->Write(reinterpret_cast<const char*>(&packet->command),  2);
    buffer->Write(reinterpret_cast<const char*>(&packet->sequence), 2);
    //buffer->write(reinterpret_cast<const char*>(&packet->data), data_size - HEADER_LEN);
    buffer->Write(packet->data, data_size - HEADER_LEN);
    
    // crc32
    uint32_t check_sum = Util::Crc32(0, reinterpret_cast<const void*>(buffer->Head()), data_size - 4);
    packet->check_sum = check_sum;

    DLOG << "encode check_sum " << check_sum;
    buffer->Write(reinterpret_cast<const char*>(&packet->check_sum), 4);

    *buf = buffer;
    // DLOG << "Buffer size " << buffer->size() << " Buffer capitiy " << buffer->capacity();
    // serializeToBuffer(message, buffer);
    // DLOG << "Buffer size " << buffer->size() << " Buffer capitiy " << buffer->capacity();
    // *buf = buffer;
}

Packet* PacketHandler::Decode(RingBuffer* buffer) {
    buffer->Adjust();
    Packet* p = new Packet;

    int data_size = buffer->GetInt16();
    // crc32
    uint32_t check_sum_val = Util::Crc32(0, reinterpret_cast<const void*>(buffer->Head()), data_size - 4); 

    int16_t length   = buffer->ReadInt16();
    int16_t version  = buffer->ReadInt16();
    uint32_t ip      = buffer->ReadUint32();
    uint16_t port    = buffer->ReadUint16();
    int16_t service  = buffer->ReadInt16();
    int16_t command  = buffer->ReadInt16();
    int16_t sequence = buffer->ReadInt16();

    char *data = new char [length - HEADER_LEN];
    buffer->ReadBytes(data, length - HEADER_LEN);

    uint32_t check_sum = (uint32_t) buffer->ReadUint32();

    DLOG << "check_sum => " << check_sum << " check_sum_val " << check_sum_val;

    // if (check_sum_val != check_sum) {
    //     DLOG << "check_sum => " << check_sum << " check_sum_val " << check_sum_val;
    //     return nullptr;
    // }  

    p->length    = length;
    p->version   = version;
    p->ip        = ip;
    p->port      = port;
    p->service   = service;
    p->command   = command;
    p->sequence  = sequence;
    p->data      = data;
    p->check_sum = check_sum;

    return p;
}

//上层数据包给网络自己复制和不复制两种选择，提高性能
// parameter message of type should be const Message& ?
void PacketHandler::SendMessage(int16_t service, int16_t command, int16_t sequence, const Message* message, const ChannelPtr& channel_ptr) {
    assert(message->ByteSize() >= 0);
    DLOG << "Message size " << message->ByteSize() << " Message name " << message->GetTypeName();
    RingBuffer* buffer;
    Packet packet;
    packet.length   = message->ByteSize() + HEADER_LEN;
    packet.version  = PROTOCOL_VERSION;
    packet.service  = service;
    packet.command  = command;
    packet.sequence = sequence;

    if (message->ByteSize() > 0) {
        packet.data = new char[message->ByteSize()];
        message->SerializeToArray(packet.data, message->ByteSize());
    } else {
        packet.data = nullptr;
    }

    Encode(&packet, &buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    channel_ptr->Send(buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();

}

void PacketHandler::SendPacket(Packet* packet) {
    RingBuffer* buffer;
    Encode(packet, &buffer);
    Send(buffer);
}

void PacketHandler::SendMessage(int16_t service, int16_t command, int16_t sequence, const Message* message) {
    assert(message->ByteSize() >= 0);
    DLOG << "Message size " << message->ByteSize() << " Message name " << message->GetTypeName();
    RingBuffer* buffer;
    Packet packet;
    packet.length   = message->ByteSize() + HEADER_LEN;
    packet.version  = PROTOCOL_VERSION;
    packet.service  = service;
    packet.command  = command;
    packet.sequence = sequence;

    if (message->ByteSize() > 0) {
        packet.data = new char[message->ByteSize()];
        message->SerializeToArray(packet.data, message->ByteSize());
    } else {
        packet.data = nullptr;
    }

    Encode(&packet, &buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    channel_ptr_->Send(buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();

}

//上层数据包给网络自己复制和不复制两种选择，提高性能
// parameter message of type should be const Message& ?
void PacketHandler::SendMessage(uint32_t ip, uint16_t port, 
    int16_t service, int16_t command, int16_t sequence, const Message* message, const ChannelPtr& channel_ptr) {
    assert(message->ByteSize() >= 0);
    DLOG << "Message size " << message->ByteSize() << " Message name " << message->GetTypeName();
    RingBuffer* buffer;
    Packet packet;
    packet.length   = message->ByteSize() + HEADER_LEN;
    packet.version  = PROTOCOL_VERSION;
    packet.ip = ip;
    packet.port = port;
    packet.service  = service;
    packet.command  = command;
    packet.sequence = sequence;

    if (message->ByteSize() > 0) {
        packet.data = new char[message->ByteSize()];
        message->SerializeToArray(packet.data, message->ByteSize());
    } else {
        packet.data = nullptr;
    }

    Encode(&packet, &buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    channel_ptr->Send(buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();

}

void PacketHandler::SendMessage(uint32_t ip, uint16_t port, 
    int16_t service, int16_t command, int16_t sequence, const Message* message) {
    
    assert(message->ByteSize() >= 0);
    DLOG << "Message size " << message->ByteSize() << " Message name " << message->GetTypeName();
    RingBuffer* buffer;
    Packet packet;
    packet.length   = message->ByteSize() + HEADER_LEN;
    packet.version  = PROTOCOL_VERSION;
    packet.ip = ip;
    packet.port = port;
    packet.service  = service;
    packet.command  = command;
    packet.sequence = sequence;

    if (message->ByteSize() > 0) {
        packet.data = new char[message->ByteSize()];
        message->SerializeToArray(packet.data, message->ByteSize());
    } else {
        packet.data = nullptr;
    }

    Encode(&packet, &buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    channel_ptr_->Send(buffer);
    DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();

}

void PacketHandler::SendPacket(Packet* packet, const ChannelPtr& channel_ptr) {
    RingBuffer* buffer;
    Encode(packet, &buffer);
    channel_ptr->Send(buffer);
}


bool PacketHandler::ParseFromBuffer(RingBuffer* buffer, Message* message) {
    BufferInputStream inputStream(buffer);
    return message->ParseFromZeroCopyStream(&inputStream);
}

bool PacketHandler::SerializeToBuffer(Message* message, RingBuffer* buffer) {
    BufferOutputStream outputStream(buffer);
    return message->SerializeToZeroCopyStream(&outputStream);
}

} // namespace marz
