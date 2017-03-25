#include "Im.Base.pb.h"
#include "ClientHandler.h"
#include "MessageAction.h"

namespace marz {

ClientHandler::ClientHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr)
{

}

ClientHandler::~ClientHandler() {
}

void ClientHandler::OnActive(const ChannelPtr& channel_ptr) {

}

void ClientHandler::OnInactive(const ChannelPtr& channel_ptr) {

}

void ClientHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet)  {
    switch (packet->command) {
        case Im::Base::HEARTBEAT:
            break;

        case Im::Base::MSG_DATA:
            Action::SendMessage(channel_ptr, packet);
            break;

        case Im::Base::REQ_MSG_LIST:
            Action::GetMessage(channel_ptr, packet);
            break;

        case Im::Base::REQ_UNREAD_COUNT:
            Action::GetUnreadMsgCounter(channel_ptr, packet);
            break;

        case Im::Base::MSG_READ_ACK:
            Action::ClearUnreadMsgCounter(channel_ptr, packet);
            break;

        case Im::Base::REQ_MSG_BY_MSG_ID:
            Action::GetMsgById(channel_ptr, packet);
            break;

        case Im::Base::REQ_LATEST_MSG_ID:
            Action::GetLatestMsgId(channel_ptr, packet);
            break;

        default:
            WLOG << "Wrong Packet, Command -> " << packet->command;
            break;
    }
}

} // namespace marz
