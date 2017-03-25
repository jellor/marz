#include "Im.Base.pb.h"
#include "ClientHandler.h"
#include "GroupAction.h"

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

        case Im::Base::REQ_NORMAL_LIST:
            Action::GetNormalGroupList(channel_ptr, packet);
            break;

        case Im::Base::REQ_GROUP_INFO:
            Action::GetGroupInfo(channel_ptr, packet);
            break;
            
        case Im::Base::REQ_CREATE_GROUP:
            Action::CreateGroup(channel_ptr, packet);
            break;
        
        case Im::Base::REQ_CHANGE_GROUP_MEMBER:
            Action::ModifyMember(channel_ptr, packet);
            break;
        
        case Im::Base::REQ_SHIELD_GROUP:
            Action::SetGroupPush(channel_ptr, packet);
            break;

        default:
            WLOG << "Wrong Packet, Command -> " << packet->command;
            break;
    }
}

} // namespace marz
