#ifndef __PACKET_HANDLER_H__
#define __PACKET_HANDLER_H__

#include <map>
#include <memory>
#include <functional>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include "Handler.h"

namespace marz {

typedef std::function<void(void)> MessageCallback;
typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
typedef google::protobuf::Message Message;
typedef google::protobuf::Descriptor Descriptor;

typedef struct Packet {
    int32_t length;
    int16_t version;
    int16_t service;
    int16_t command;
    int16_t sequence;
    int16_t reserve;
    char* 	data;
    int32_t check_sum;

    ~Packet() {
    	if (data != NULL) {
    		DLOG << "Free packet.data ...";
    		delete data;
    	}
    }

} Packet;

class PacketHandler : public Handler {
public:
    PacketHandler(const ChannelPtr& channel_ptr): Handler(channel_ptr) {}

	virtual void OnReceive(const ChannelPtr& channel_ptr) override;
	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) = 0;

	void SendPacket(Packet* packet, const ChannelPtr& channel_ptr);
    void SendMessage(int16_t service, int16_t command, int16_t sequence, const Message* message);
	static void SendMessage(int16_t service, int16_t command, int16_t sequence, const Message* message, const ChannelPtr& channel_ptr);

private:
	bool Valid(RingBuffer* buffer);
	static void Encode(Packet* packet, RingBuffer** buffer);
	Packet* Decode(RingBuffer* buffer);

	bool ParseFromBuffer(RingBuffer* buffer, Message* message);
	bool SerializeToBuffer(Message* message, RingBuffer* buffer);

	static const int16_t PROTOCOL_VERSION = 1;

};

#define PRINT_PACKET(packet)							\
	do {												\
   		DLOG << "[LEN : " << packet->length << ", "	\
            << "VER : " << packet->version << ", "		\
            << "SER : " << packet->service << ", "		\
            << "CMD : " << packet->command << ", "		\
            << "SEQ : " << packet->sequence << ", "		\
            << "RES : " << packet->reserve << ", "		\
            << "CHE : " << packet->checkSum<<"]";		\
	} while (0)

#define PARSE_PACKET(msg, packet)	(msg).ParseFromArray((packet)->data, (packet)->length - 18)

} // marz

#endif // __PACKET_HANDLER_H__
