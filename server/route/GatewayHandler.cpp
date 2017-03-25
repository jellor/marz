#include "protocolbuffer/Im.Base.pb.h"
#include "protocolbuffer/Im.Login.pb.h"
#include "protocolbuffer/Im.Server.pb.h"
#include "protocolbuffer/Im.Message.pb.h"
#include "protocolbuffer/Im.SwitchService.pb.h"
#include "protocolbuffer/Im.Buddy.pb.h"
#include "Timestamp.h"
#include "GatewayHandler.h"
#include "GatewayMgr.h"
#include "Singleton.h"
#include "LoginMgr.h"
#include "GroupMgr.h"
#include "MsgMgr.h"
#include "RelationMgr.h"
#include "SessionMgr.h"
#include "UserMgr.h"

extern std::atomic<uint64_t> g_msg_cnt_per_sec; 

namespace marz {

GatewayHandler::GatewayHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
peer_addr_(),
addr_set_()
{
    DLOG << "GatewayHandler Constructor";
}

GatewayHandler::~GatewayHandler() {
    DLOG << "GatewayHandler Destructor";
}

void GatewayHandler::OnActive(const ChannelPtr& channel_ptr) {

    peer_addr_ = channel_ptr->GetPeerAddress().ToString();
    GatewayMgr& gateway_mgr = Singleton<GatewayMgr>::GetInstance();
    gateway_mgr.AddHandler(peer_addr_, this);
    channel_ptr->GetEventLoop()->AddTimer(std::bind(&GatewayHandler::OnTimeout, this), 
        Timestamp::Now().Macrosecond(), 5, 1);
}

void GatewayHandler::OnInactive(const ChannelPtr& channel_ptr) {
    GatewayMgr& gateway_mgr = Singleton<GatewayMgr>::GetInstance();
    gateway_mgr.DelHandler(peer_addr_);
    Close();
}

void GatewayHandler::OnTimeout(){
    WLOG << "OnTimeout";
}

void GatewayHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {

    std::string addr = IpAddress::ToString(packet->ip, packet->port);
    if (addr_set_.find(addr) == addr_set_.end()) {
        addr_set_.insert(addr);
    }

    switch (packet->service) {
        case Im::Base::HEARTBEAT:
            HandleHeartBeat(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_LOGIN:
            HandleLogin(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_GROUP:
            HandleGroup(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_MESSAGE:
            HandleMessage(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_RELATION:
            HandleRelation(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_SESSION:
            HandleSession(channel_ptr, packet);
            break;
        case Im::Base::SERVICE_USER:
            HandleUser(channel_ptr, packet);
            break;
        
        default:
            WLOG << "packet command => " << packet->command;
            break;
    }
}

void GatewayHandler::SendToGatewaySvr(Packet* packet) {
    std::string addr = IpAddress::ToString(packet->ip, packet->port);
    if (addr_set_.find(addr) != addr_set_.end()) {
        SendPacket(packet);
    }
}

void GatewayHandler::HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet) {
    DLOG << "Heart Beat";
}

void GatewayHandler::HandleLogin(const ChannelPtr& channel_ptr, Packet* packet) 
{
    DLOG << "Login";
    
    LoginMgr& login_mgr = Singleton<LoginMgr>::GetInstance();
    login_mgr.SendToLoginSvr(packet);
    
}

void GatewayHandler::HandleGroup(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "Group";

    GroupMgr& group_mgr = Singleton<GroupMgr>::GetInstance();
    group_mgr.SendToGroupSvr(packet);
}

void GatewayHandler::HandleMessage(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "Message";
    
    MsgMgr& msg_mgr = Singleton<MsgMgr>::GetInstance();
    msg_mgr.SendToMsgSvr(packet);
}   

void GatewayHandler::HandleRelation(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "Relation";
    
    RelationMgr& relation_mgr = Singleton<RelationMgr>::GetInstance();
    relation_mgr.SendToRelationSvr(packet);
}

void GatewayHandler::HandleSession(const ChannelPtr& channel_ptr, Packet *packet)
{
    DLOG << "Session";

    SessionMgr& session_mgr = Singleton<SessionMgr>::GetInstance();
    session_mgr.SendToSessionSvr(packet);
}

void GatewayHandler::HandleUser(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "User";

    UserMgr& user_mgr = Singleton<UserMgr>::GetInstance();
    user_mgr.SendToUserSvr(packet);
}

} // namespace marz
