#include "ClientHandler.h"
#include "LoginAction.h"

namespace marz {

ClientHandler::ClientHandler(const ChannelPtr& channel_ptr):
Handler(channel_ptr),
http_context_()
{
    http_context_.SetHttpCallback(std::bind(&ClientHandler::OnHttp, this, std::placeholders::_1));
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

        case Im::Base::REQ_VALIDATE:
            Action::Login(channel_ptr, packet);
            break;

        default:
            WLOG << "Wrong Packet, Command -> " << packet->command;
            break;
    }
}

} // namespace marz
