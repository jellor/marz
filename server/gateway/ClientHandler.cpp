#include "protobuf/Im.Base.pb.h"
#include "protobuf/Im.Login.pb.h"
#include "protobuf/Im.Server.pb.h"
#include "protobuf/Im.Message.pb.h"
#include "protobuf/Im.SwitchService.pb.h"
#include "protobuf/Im.Buddy.pb.h"
#include "ClientHandler.h"
#include "Singleton.h"
#include "RouteMgr.h"

extern std::atomic<uint64_t> g_msg_cnt_per_sec; 

namespace marz {

ClientHandler::ClientHandler(const ChannelPtr& channel_ptr):
PacketHandler(channel_ptr),
id_(-1),
logined_(false)
{
    //http_context_.setHttpCallback(std::bind(&ClientHandler::onHttp, this, std::placeholders::_1));
}

ClientHandler::~ClientHandler() {

}

void ClientHandler::onActive(const ChannelPtr& channel_ptr) {
    id_ = channel_ptr->GetSocket()->Fd();
}

void ClientHandler::onInactive(const ChannelPtr& channel_ptr) {
    
}

void ClientHandler::onMessage(const ChannelPtr& channel_ptr, Packet* packet) {
    // authorization check.
    if (!Check(packet)) {
        WARN << "Authorization Check Failed";
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
            handleDeviceToken(channel_ptr, packet);
            break;
        case Im::Base::KICK_USER:
            handleKickUser(channel_ptr, packet);
            break;
        case Im::Base::MSG_DATA:
            handleMsgData(channel_ptr, packet);
            break;
        case Im::Base::MSG_DATA_ACK:
            handleMsgDataAck(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_TIME:
            handleMsgTime(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_LIST:
            handleMsgList(channel_ptr, packet);
            break;
        case Im::Base::REQ_MSG_BY_MSG_ID:
            handleMsgByMsgId(channel_ptr, packet);
            break;
        case Im::Base::REQ_UNREAD_COUNT:
            handleUnreadCount(channel_ptr, packet);
            break;
        case Im::Base::MSG_READ_ACK:
            handleReadAck(channel_ptr, packet);
            break;
        case Im::Base::REQ_LATEST_MSG_ID:
            handleLatestMsgId(channel_ptr, packet);
            break;
        case Im::Base::P2P_CMD:
            handleP2PCommand(channel_ptr, packet);
            break;
        case Im::Base::REQ_RECENT_CONTACT_SESSION:
            handleRecentContactSession(channel_ptr, packet);
            break;
        case Im::Base::REQ_USER_INFO:
            handleUserInfo(channel_ptr, packet);
            break;
        case Im::Base::REQ_REMOVE_SESSION:
            handleRemoveSession(channel_ptr, packet);
            break;
        case Im::Base::REQ_ALL_USER:
            handleAllUser(channel_ptr, packet);
            break;
        case Im::Base::REQ_CHANGE_AVATAR:
            handleChangeAvatar(channel_ptr, packet);
            break;
        case Im::Base::REQ_USERS_STATUS:
            handleUsersStatus(channel_ptr, packet);
            break;

        // for group.
        case Im::Base::REQ_NORMAL_LIST:
            handleNormalList(channel_ptr, packet);
            break;
        case Im::Base::REQ_GROUP_INFO:
            handleGroupList(channel_ptr, packet);
            break;
        case Im::Base::REQ_CREATE_GROUP:
            handleCreateGroup(channel_ptr, packet);
            break;
        case Im::Base::REQ_CHANGE_GROUP_MEMBER:
            handleChangeGroupMember(channel_ptr, packet);
            break;
        case Im::Base::REQ_SHIELD_GROUP:
            handleShieldGroup(channel_ptr, packet);
            break;

        default:
            WARN << "packet command => " << packet->command;
            break;
    }
}

bool ClientHandler::Check(Packet *packet) {
    if (packet->command != Im::Base::REQ_USER_LOGIN && !logined_) {
        return false;
    }
}

void ClientHandler::HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet) {
    DEBUG << "Heart Beat";
}

void ClientHandler::HandleUserLogin(const ChannelPtr& channel_ptr, Packet* packet)
{
    DEBUG << "User Login";
    // refuse double check login validate.
    if (user_name_.length() != 0 || user_password_.length() != 0) {
        DEBUG << "Double Login In Same Connection";
        return;
    }
    // check if all server connection are available.
    /* dao servers */
    /* balance servers */
    /* route servers */

    Im::Login::LoginRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);

    DEBUG << msg.user_name() << " | " << msg.user_password() << " | " << msg.online_status() 
        << " | " << msg.client_type();
    user_name_ = msg.user_name();
    user_password_ = msg.user_password();
    uint32_t online_status = msg.online_status();
    if (online_status < Im::Base::USER_STATUS_ONLINE || online_status > Im::Base::USER_STATUS_LEAVE) {
        DEBUG << "Online Status Failed => " << online_status;
        online_status = Im::Base::USER_STATUS_ONLINE;
    }

    Im::Server::ValidateRequest request;
    request.set_user_name(user_name_);
    request.set_user_password(user_password_);

    RouteMgr& route_mgr = Singleton<RouteMgr>::GetInstance();
    route_mgr.SendMsgToRouteSvr(Im::Base::SERVICE_LOGIN, Im::Base::REQ_USER_LOGIN, packet->sequence, request);
    
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

    SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_USER_LOGIN, packet->sequence, &login_response, channel_ptr);
}

void ClientHandler::handleUserLogout(const ChannelPtr& channel_ptr, Packet* packet)
{
    DEBUG << "user id => " << user_id_ << " client type => " << client_type_;
    // send to dao server about logout.

    // response to client.
    Im::Login::LogoutResponse logout_res;
    logout_res.set_result_code(0);
    sendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_USER_LOGOUT, packet->sequence, &logout_res, channel_ptr);
}

void ClientHandler::handleDeviceToken(const ChannelPtr& channel_ptr, Packet* packet)
{
    DEBUG << "client_type => " << client_type_;
    Im::Login::DeviceTokenRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    std::string device_token = msg.device_token();
    DEBUG << "user id => " << user_id_ << " device_token => " << device_token;

    // send to client.
    Im::Login::DeviceTokenResponse device_token_res;
    device_token_res.set_user_id(user_id_);
    device_token_res.set_attach_data("attach data");
    sendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_DEVICE_TOKEN, packet->sequence, &device_token_res, channel_ptr);

    // send to dao server.
    Im::Login::DeviceTokenRequest request;
    request.set_user_id(user_id_);
    request.set_device_token(device_token);
    request.set_client_type((Im::Base::ClientType)client_type_);
    request.set_attach_data("attach data");
    sendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_DEVICE_TOKEN, packet->sequence, &request, channel_ptr);
}   

void ClientHandler::handleKickUser(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Login::KickPCClientRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    DEBUG << "kick user id => " << user_id;
    // send to route server.

    // response to client.
    Im::Login::KickPCCLientResponse response;
    response.set_user_id(user_id_);
    response.set_result_code(0);
    sendMessage(Im::Base::SERVICE_LOGIN, Im::Base::KICK_USER, packet->sequence, &response, channel_ptr);
}

void ClientHandler::handleMsgData(const ChannelPtr& channel_ptr, Packet *packet)
{
    Im::Message::MessageData msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    if (msg.msg_data().length() == 0) {
        DEBUG << "discard data in user id => " << user_id_;
        return;
    }
    if (msg.from_user_id() == msg.to_session_id()) {
        DEBUG << "from user id => " << msg.from_user_id() << " to session id => " << msg.to_session_id();
        return;
    }
    g_msg_cnt_per_sec ++;

    uint32_t msg_id = msg.msg_id();
    uint32_t to_session_id = msg.to_session_id();
    uint32_t msg_type = msg.msg_type();
    std::string msg_data = msg.msg_data();
    DEBUG << "msg id => " << msg_id << " from user id => " << msg.from_user_id() << " to session id => " << to_session_id;

    // send to dao server.
    uint32_t cur_time = time(NULL);
    msg.set_from_user_id(user_id_);
    msg.set_create_time(cur_time);
    msg.set_attach_data("attach data");
}

void ClientHandler::handleMsgDataAck(const ChannelPtr& channel_ptr, Packet* packet)
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

void ClientHandler::handleMsgTime(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::ClientTimeResponse msg;
    msg.set_server_time((uint32_t) time(NULL));
    sendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_MSG_TIME, packet->sequence, &msg, channel_ptr);
}

void ClientHandler::handleMsgList(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetMsgListRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t msg_id_begin = msg.msg_id_begin();
    uint32_t msg_count = msg.msg_count();
    uint32_t session_type = msg.session_type();
    DEBUG << "session id => " << session_id << " msg id begin => " << msg_id_begin
        << " msg count => " << msg_count << " session type => " << session_type;
    // send to dao server.

}

void ClientHandler::handleMsgByMsgId(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetLatestMsgIdResquest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    DEBUG << "session id => " << session_id  << session_type;
    // send to dao server.

}

void ClientHandler::handleUnreadCount(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::UnreadMsgCntRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DEBUG << "user id => " << msg.user_id();
    // send to dao server.
}

void ClientHandler::handleReadAck(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::MessageDataReadAck msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    uint32_t msg_id = msg.msg_id();
    DEBUG << "session id => " << session_id << " session type => " << session_type << " msg id => " << msg_id;
    // send to dao server.

    // notify peer user client.
    Im::Message::MessageDataReadNotify notify;
    notify.set_user_id(user_id_);
    notify.set_session_id(session_id);
    notify.set_session_type((Im::Base::SessionType)session_type);
    notify.set_msg_id(msg_id);

    // and broadcast route server. 

}

void ClientHandler::handleLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Message::GetLatestMsgIdResquest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_type = msg.session_type();
    uint32_t session_id = msg.session_id();
    DEBUG << "session id => " << session_id << " session type => " << session_type;
    // send to dao server.
}

void ClientHandler::handleP2PCommand(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::SwitchService::P2PCommand msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t from_user_id = msg.from_user_id();
    uint32_t to_user_id = msg.to_user_id();
    DEBUG << "from user id => " << from_user_id << " to user id => " << to_user_id;

    // send to from_user_id and to_user_id.

    // send to route server.
}

void ClientHandler::handleRecentContactSession(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::RecentContactSessionRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    uint32_t latest_update_time = msg.latest_update_time();
    DEBUG << "user id => " << user_id << " latest update time => " << latest_update_time;

    // send to dao server.
}

void ClientHandler::handleUserInfo(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::UserInfoRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t user_id = msg.user_id();
    DEBUG << "user id => " << user_id << " user id list size => " << msg.user_id_list_size();
    // send to dao server.

}

void ClientHandler::handleRemoveSession(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::RemoveSessionRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t session_id = msg.session_id();
    uint32_t session_type = msg.session_type();
    DEBUG << "session id => " << session_id << " session_type => " << session_type;

    // send to dao server.


    // send to user_id and route server.
    if (session_type == Im::Base::SESSION_TYPE_SINGLE) {
        Im::Buddy::RemoveSessionNotify notify;
        notify.set_user_id(user_id_);
        notify.set_session_id(session_id);
        notify.set_session_type((Im::Base::SessionType)session_type);
    }
}

void ClientHandler::handleAllUser(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::AllUserRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    uint32_t latest_update_time = msg.latest_update_time();
    DEBUG << "latest update time => " << latest_update_time;

    // send to dao server.
}

void ClientHandler::handleChangeAvatar(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::changeAvatarRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DEBUG << "change avatar id => " << user_id_;

    // send to dao server.
}

void ClientHandler::handleUsersStatus(const ChannelPtr& channel_ptr, Packet* packet)
{
    Im::Buddy::UsersStatusRequest msg;
    msg.ParseFromArray(packet->data, packet->length - 18);
    DEBUG << "user id list size => " << msg.user_id_list_size();

    // send to route server.
}

void ClientHandler::handleNormalList(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::handleGroupList(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::handleCreateGroup(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::handleChangeGroupMember(const ChannelPtr& channel_ptr, Packet* packet)
{

}

void ClientHandler::handleShieldGroup(const ChannelPtr& channel_ptr, Packet* packet)
{

}


} // namespace marz
