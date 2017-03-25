#include "protocolbuffer/Im.Base.pb.h"
#include "protocolbuffer/Im.Login.pb.h"
#include "protocolbuffer/Im.Server.pb.h"
#include "protocolbuffer/Im.Message.pb.h"
#include "protocolbuffer/Im.SwitchService.pb.h"
#include "protocolbuffer/Im.Buddy.pb.h"
#include "ClientHandler.h"
#include "Singleton.h"
#include "RouteMgr.h"
#include "ClientMgr.h"

namespace marz {

extern std::atomic<uint64_t> g_msg_cnt_per_sec; 

ClientHandler::ClientHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
logined_(false),
client_addr_()
{
    //http_context_.setHttpCallback(std::bind(&ClientHandler::onHttp, this, std::placeholders::_1));
}

ClientHandler::~ClientHandler() {

}

void ClientHandler::OnActive(const ChannelPtr& channel_ptr) {
    id_ = channel_ptr->GetSocket()->Fd();
    client_addr_ = channel_ptr->GetPeerAddress();
    ClientMgr& client_mgr = Singleton<ClientMgr>::GetInstance();
    client_mgr.AddHandler(client_addr_.ToString(), this);
}

void ClientHandler::OnInactive(const ChannelPtr& channel_ptr) {
    ClientMgr& client_mgr = Singleton<ClientMgr>::GetInstance();
    client_mgr.DelHandler(client_addr_.ToString());
    Close();
}

void ClientHandler::OnMessage(const ChannelPtr& channel_ptr, Packet* packet) {
    // authorization check.
    if (!Check(packet)) {
        WLOG << "Authorization Check Failed";
        return;
    }

    switch (packet->command) {
        case Im::Base::HEARTBEAT:
            HandleHeartBeat(channel_ptr, packet);
            break;
        case Im::Base::REQ_USER_LOGIN:
            HandleUserLogin(channel_ptr, packet);
            break;
        case Im::Base::REQ_USER_LOGOUT:
            HandleUserLogout(channel_ptr, packet);
            break;
        case Im::Base::REQ_GET_DEVICE_TOKEN:
            HandleDeviceToken(channel_ptr, packet);
            break;
        case Im::Base::KICK_USER:
            HandleKickUser(channel_ptr, packet);
            break;
        case Im::Base::MSG_DATA:
            HandleMsgData(channel_ptr, packet);
            break;
        case Im::Base::MSG_DATA_ACK:
            HandleMsgDataAck(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_TIME:
            HandleMsgTime(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_LIST:
            HandleMsgList(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_BY_MSG_ID:
            HandleMsgByMsgId(channel_ptr, packet);
            break;
        case Im::Base::REQ_UNREAD_COUNT:
            HandleUnreadCount(channel_ptr, packet);
            break;
        case Im::Base::MSG_READ_ACK:
            HandleReadAck(channel_ptr, packet);
            break;
        case Im::Base::REQ_LATEST_MSG_ID:
            HandleLatestMsgId(channel_ptr, packet);
            break;
        case Im::Base::P2P_CMD:
            HandleP2PCommand(channel_ptr, packet);
            break;
        case Im::Base::REQ_RECENT_CONTACT_SESSION:
            HandleRecentContactSession(channel_ptr, packet);
            break;
        case Im::Base::REQ_USER_INFO:
            HandleUserInfo(channel_ptr, packet);
            break;
        case Im::Base::REQ_REMOVE_SESSION:
            HandleRemoveSession(channel_ptr, packet);
            break;
        case Im::Base::REQ_ALL_USER:
            HandleAllUser(channel_ptr, packet);
            break;
        case Im::Base::REQ_CHANGE_AVATAR:
            HandleChangeAvatar(channel_ptr, packet);
            break;
        case Im::Base::REQ_USERS_STATUS:
            HandleUsersStatus(channel_ptr, packet);
            break;

        // for group.
        case Im::Base::REQ_NORMAL_LIST:
            HandleNormalList(channel_ptr, packet);
            break;
        case Im::Base::REQ_GROUP_INFO:
            HandleGroupList(channel_ptr, packet);
            break;
        case Im::Base::REQ_CREATE_GROUP:
            HandleCreateGroup(channel_ptr, packet);
            break;
        case Im::Base::REQ_CHANGE_GROUP_MEMBER:
            HandleChangeGroupMember(channel_ptr, packet);
            break;
        case Im::Base::REQ_SHIELD_GROUP:
            HandleShieldGroup(channel_ptr, packet);
            break;

        default:
            WLOG << "packet command => " << packet->command;
            break;
    }
}

bool ClientHandler::Check(Packet *packet) {
    if (packet->command != Im::Base::REQ_USER_LOGIN && !logined_) {
        return false;
    } else {
        return true;
    }
}

void ClientHandler::HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet) {
    DLOG << "Heart Beat";
}

void ClientHandler::HandleUserLogin(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "User Login";
    // refuse double check login validate.
    if (user_name_.length() != 0 || user_password_.length() != 0) {
        DLOG << "Double Login In Same Connection";
        return;
    }
    // check if all server connection are available.
    /* dao servers */
    /* balance servers */
    /* route servers */

    Im::Login::LoginRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);

    DLOG << msg.user_name() << " | " << msg.user_password() << " | " << msg.online_status() 
        << " | " << msg.client_type();
    user_name_ = msg.user_name();
    user_password_ = msg.user_password();
    uint32_t online_status = msg.online_status();
    if (online_status < Im::Base::USER_STATUS_ONLINE || online_status > Im::Base::USER_STATUS_LEAVE) {
        DLOG << "Online Status Failed => " << online_status;
        online_status = Im::Base::USER_STATUS_ONLINE;
    }

    Im::Login::ValidateRequest request;
    request.set_user_name(user_name_);
    request.set_user_password(user_password_);

    RouteMgr& route_mgr = Singleton<RouteMgr>::GetInstance();
    route_mgr.SendToRouteSvr(client_addr_.Ip(), client_addr_.Port(),
        Im::Base::SERVICE_LOGIN, Im::Base::REQ_VALIDATE, packet->sequence, &request);
    
    // response to client should place in dao handler.
    Im::Login::LoginResponse login_response;
    login_response.set_server_time(time(NULL));
    login_response.set_result_code(Im::Base::REFUSE_REASON_NONE);
    login_response.set_result_string("ok");
    login_response.set_online_status(Im::Base::USER_STATUS_ONLINE);
    Im::Base::UserInfo *user_info = login_response.mutable_user_info();
    user_info->set_user_id(100);
    user_info->set_sex(111);
    user_info->set_nick("guodong");
    user_info->set_avatar("http://baidu.com");
    user_info->set_phone("1888888");
    user_info->set_email("842352268@qq.com");
    user_info->set_name("jellor");
    user_info->set_domain("jjkkjk");
    user_info->set_status(uint32_t(1));

    SendMessage(client_addr_.Ip(), client_addr_.Port(), 
        Im::Base::SERVICE_LOGIN, Im::Base::RES_VALIDATE, packet->sequence, &login_response);
}

void ClientHandler::HandleUserLogout(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "user id => " << user_id_ << " client type => " << client_type_;
    // send to dao server about logout.

    // response to client.
    Im::Login::LogoutResponse logout_res;
    logout_res.set_result_code(0);
    SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_USER_LOGOUT, packet->sequence, &logout_res, channel_ptr);
}

void ClientHandler::HandleDeviceToken(const ChannelPtr& channel_ptr, Packet* packet)
{
    DLOG << "client_type => " << client_type_;
    Im::Login::DeviceTokenRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    std::string device_token = msg.device_token();
    DLOG << "user id => " << user_id_ << " device_token => " << device_token;

    // send to client.
    Im::Login::DeviceTokenResponse device_token_res;
    device_token_res.set_user_id(user_id_);
    device_token_res.set_attach_data("attach data");
    SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_DEVICE_TOKEN, packet->sequence, &device_token_res, channel_ptr);

    // send to dao server.
    Im::Login::DeviceTokenRequest request;
    request.set_user_id(user_id_);
    request.set_device_token(device_token);
    request.set_client_type((Im::Base::ClientType)client_type_);
    request.set_attach_data("attach data");
    SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_DEVICE_TOKEN, packet->sequence, &request, channel_ptr);
}   

void ClientHandler::HandleKickUser(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Login::KickPCClientRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    DLOG << "kick user id => " << user_id;
    // send to route server.

    // response to client.
    Im::Login::KickPCCLientResponse response;
    response.set_user_id(user_id_);
    response.set_result_code(0);
    SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::KICK_USER, packet->sequence, &response, channel_ptr);
}

void ClientHandler::HandleMsgData(const ChannelPtr& channel_ptr, Packet *packet)
{
    Im::Message::MessageData msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    if (msg.msg_data().length() == 0) {
        DLOG << "discard data in user id => " << user_id_;
        return;
    }
    if (msg.from_user_id() == msg.to_session_id()) {
        DLOG << "from user id => " << msg.from_user_id() << " to session id => " << msg.to_session_id();
        return;
    }
    g_msg_cnt_per_sec ++;

    uint32_t msg_id = msg.msg_id();
    uint32_t to_session_id = msg.to_session_id();
    uint32_t msg_type = msg.msg_type();
    std::string msg_data = msg.msg_data();
    DLOG << "msg id => " << msg_id << " from user id => " << msg.from_user_id() << " to session id => " << to_session_id;

    // send to dao server.
    uint32_t cur_time = time(NULL);
    msg.set_from_user_id(user_id_);
    msg.set_create_time(cur_time);
    msg.set_attach_data("attach data");
}

void ClientHandler::HandleMsgDataAck(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::MessageDataAck msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    Im::Base::SessionType session_type = msg.session_type();
    if (session_type == Im::Base::SESSION_TYPE_SINGLE) {
        uint32_t session_id = msg.session_id();
        uint32_t msg_id = msg.msg_id();

        // del from send list.
    }
}

void ClientHandler::HandleMsgTime(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::ClientTimeResponse msg;
    msg.set_server_time((uint32_t) time(NULL));
    SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_MSG_TIME, packet->sequence, &msg, channel_ptr);
}

void ClientHandler::HandleMsgList(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetMsgListRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t msg_id_begin = msg.msg_id_begin();
    uint32_t msg_count = msg.msg_count();
    uint32_t session_type = msg.session_type();
    DLOG << "session id => " << session_id << " msg id begin => " << msg_id_begin
        << " msg count => " << msg_count << " session type => " << session_type;
    // send to dao server.

}

void ClientHandler::HandleMsgByMsgId(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetLatestMsgIdResquest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    DLOG << "session id => " << session_id  << session_type;
    // send to dao server.

}

void ClientHandler::HandleUnreadCount(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::UnreadMsgCntRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DLOG << "user id => " << msg.user_id();
    // send to dao server.
}

void ClientHandler::HandleReadAck(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::MessageDataReadAck msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    uint32_t msg_id = msg.msg_id();
    DLOG << "session id => " << session_id << " session type => " << session_type << " msg id => " << msg_id;
    // send to dao server.

    // notify peer user client.
    Im::Message::MessageDataReadNotify notify;
    notify.set_user_id(user_id_);
    notify.set_session_id(session_id);
    notify.set_session_type((Im::Base::SessionType)session_type);
    notify.set_msg_id(msg_id);

    // and broadcast route server. 

}

void ClientHandler::HandleLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetLatestMsgIdResquest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    DLOG << "session id => " << session_id << " session type => " << session_type;
    // send to dao server.
}

void ClientHandler::HandleP2PCommand(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::SwitchService::P2PCommand msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t from_user_id = msg.from_user_id();
    uint32_t to_user_id = msg.to_user_id();
    DLOG << "from user id => " << from_user_id << " to user id => " << to_user_id;

    // send to from_user_id and to_user_id.

    // send to route server.
}

void ClientHandler::HandleRecentContactSession(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::RecentContactSessionRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    uint32_t latest_update_time = msg.latest_update_time();
    DLOG << "user id => " << user_id << " latest update time => " << latest_update_time;

    // send to dao server.
}

void ClientHandler::HandleUserInfo(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::UserInfoRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    DLOG << "user id => " << user_id << " user id list size => " << msg.user_id_list_size();
    // send to dao server.

}

void ClientHandler::HandleRemoveSession(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::RemoveSessionRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    DLOG << "session id => " << session_id << " session_type => " << session_type;

    // send to dao server.


    // send to user_id and route server.
    if (session_type == Im::Base::SESSION_TYPE_SINGLE) {
        Im::Buddy::RemoveSessionNotify notify;
        notify.set_user_id(user_id_);
        notify.set_session_id(session_id);
        notify.set_session_type((Im::Base::SessionType)session_type);
    }
}

void ClientHandler::HandleAllUser(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::AllUserRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t latest_update_time = msg.latest_update_time();
    DLOG << "latest update time => " << latest_update_time;

    // send to dao server.
}

void ClientHandler::HandleChangeAvatar(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::changeAvatarRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DLOG << "change avatar id => " << user_id_;

    // send to dao server.
}

void ClientHandler::HandleUsersStatus(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::UsersStatusRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DLOG << "user id list size => " << msg.user_id_list_size();

    // send to route server.
}

void ClientHandler::HandleNormalList(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::HandleGroupList(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::HandleCreateGroup(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::HandleChangeGroupMember(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::HandleShieldGroup(const ChannelPtr& channel_ptr, Packet* packet)
{

}


} // namespace marz
