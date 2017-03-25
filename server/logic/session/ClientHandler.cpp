#include "Im.Base.pb.h"
#include "ClientHandler.h"
#include "SessionAction.h"

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

        case Im::Base::REQ_RECENT_SESSION:
            Action::GetRecentSession(channel_ptr, packet);
            break;

        case Im::Base::REQ_REMOVE_SESSION:
            Action::DelRecentSession(channel_ptr, packet);
            break;

        default:
            WLOG << "Wrong Packet, Command -> " << packet->command;
            break;
    }
}

} // namespace marz
