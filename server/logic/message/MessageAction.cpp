#include <list>
#include <vector>
#include <map>
#include "Im.Message.pb.h"
#include "Im.Login.pb.h"
#include "Im.Server.pb.h"
#include "MessageAction.h"
#include "MessageModel.h"
#include "GroupMessageModel.h"
#include "GroupModel.h"
#include "UserModel.h"
#include "Util.h"
#include "CacheManager.h"
#include "Logger.h"
#include "SessionModel.h"
#include "RelationModel.h"
#include "Types.h"

namespace marz {

namespace Action {

void SendMessage(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::MessageData msg;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t from_user_id = msg.from_user_id();
        uint32_t to_user_id = msg.to_session_id();
        uint32_t create_time = msg.create_time();
        Im::Base::MsgType msg_type = msg.msg_type();
        uint32_t msg_len = msg.msg_data().length();
        
        uint32_t now = (uint32_t)time(NULL);
        if (Im::Base::MsgType_IsValid(msg_type)) {
            if (msg_len != 0) {
                uint32_t msg_id = INVALID_VALUE;
                uint32_t session_id = INVALID_VALUE;
                uint32_t peer_session_id = INVALID_VALUE;

                MessageModel* msg_model = MessageModel::GetInstance();
                GroupMessageModel* group_msg_model = GroupMessageModel::GetInstance();
                if (msg_type == Im::Base::MSG_TYPE_GROUP_TEXT) {
                    GroupModel* group_model = GroupModel::GetInstance();
                    if (group_model->IsInvalidateGroupId(to_user_id) && group_model->IsInGroup(from_user_id, to_user_id)) {
                        session_id = SessionModel::GetInstance()->GetSessionId(from_user_id, to_user_id, Im::Base::SESSION_TYPE_GROUP, false);
                        if (INVALID_VALUE == session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(from_user_id, to_user_id, Im::Base::SESSION_TYPE_GROUP);
                        }
                        if (session_id != INVALID_VALUE) {
                            msg_id = group_msg_model->GetMsgId(to_user_id);
                            if (msg_id != INVALID_VALUE) {
                                group_msg_model->SendMessage(from_user_id, to_user_id, msg_type, create_time, msg_id, (std::string&)msg.msg_data());
                                SessionModel::GetInstance()->UpdateSession(session_id, now);
                            }
                        }
                    } else {
                        //log("invalid groupId. fromId=%u, groupId=%u", from_user_id, to_user_id);
                        return;
                    }
                } else if (msg_type == Im::Base::MSG_TYPE_GROUP_AUDIO) {
                    GroupModel* group_model = GroupModel::GetInstance();
                    if (group_model->IsInvalidateGroupId(to_user_id)&& group_model->IsInGroup(from_user_id, to_user_id)) {
                        session_id = SessionModel::GetInstance()->GetSessionId(from_user_id, to_user_id, Im::Base::SESSION_TYPE_GROUP, false);
                        if (INVALID_VALUE == session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(from_user_id, to_user_id, Im::Base::SESSION_TYPE_GROUP);
                        }
                        if (session_id != INVALID_VALUE) {
                            msg_id = group_msg_model->GetMsgId(to_user_id);
                            if (msg_id != INVALID_VALUE) {
                                group_msg_model->SendAudioMessage(from_user_id, to_user_id, msg_type, create_time, msg_id, msg.msg_data().c_str(), msg_len);
                                SessionModel::GetInstance()->UpdateSession(session_id, now);
                            }
                        }
                    } else {
                        //log("invalid groupId. fromId=%u, groupId=%u", from_user_id, to_user_id);
                        
                        return;
                    }
                } else if(msg_type== Im::Base::MSG_TYPE_SINGLE_TEXT) {
                    if (from_user_id != to_user_id) {
                        session_id = SessionModel::GetInstance()->GetSessionId(from_user_id, to_user_id, Im::Base::SESSION_TYPE_SINGLE, false);
                        if (INVALID_VALUE == session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(from_user_id, to_user_id, Im::Base::SESSION_TYPE_SINGLE);
                        }
                        peer_session_id = SessionModel::GetInstance()->GetSessionId(to_user_id, from_user_id, Im::Base::SESSION_TYPE_SINGLE, false);
                        if (INVALID_VALUE ==  peer_session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(to_user_id, from_user_id, Im::Base::SESSION_TYPE_SINGLE);
                        }
                        uint32_t relate_id = RelationModel::GetInstance()->GetRelationId(from_user_id, to_user_id, true);
                        if (session_id != INVALID_VALUE && relate_id != INVALID_VALUE) {
                            msg_id = msg_model->GetMsgId(relate_id);
                            if (msg_id != INVALID_VALUE) {
                                msg_model->SendMessage(relate_id, from_user_id, to_user_id, msg_type, create_time, msg_id, (std::string&)msg.msg_data());
                                SessionModel::GetInstance()->UpdateSession(session_id, now);
                                SessionModel::GetInstance()->UpdateSession(peer_session_id, now);
                            } else {
                                //log("msgId is invalid. fromId=%u, toId=%u, relate_id=%u, session_id=%u, msg_type=%u", from_user_id, to_user_id, relate_id, session_id, msg_type);
                            }
                        } else {
                            //log("sessionId or relateId is invalid. fromId=%u, toId=%u, relate_id=%u, session_id=%u, msg_type=%u", from_user_id, to_user_id, relate_id, session_id, msg_type);
                        }
                    } else {
                        //log("send msg to self. fromId=%u, toId=%u, msgType=%u", from_user_id, to_user_id, msg_type);
                    }
                    
                } else if(msg_type == Im::Base::MSG_TYPE_SINGLE_AUDIO) {
                    
                    if (from_user_id != to_user_id) {
                        session_id = SessionModel::GetInstance()->GetSessionId(from_user_id, to_user_id, Im::Base::SESSION_TYPE_SINGLE, false);
                        if (INVALID_VALUE == session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(from_user_id, to_user_id, Im::Base::SESSION_TYPE_SINGLE);
                        }
                        peer_session_id = SessionModel::GetInstance()->GetSessionId(to_user_id, from_user_id, Im::Base::SESSION_TYPE_SINGLE, false);
                        if (INVALID_VALUE ==  peer_session_id) {
                            session_id = SessionModel::GetInstance()->AddSession(to_user_id, from_user_id, Im::Base::SESSION_TYPE_SINGLE);
                        }
                        uint32_t relate_id = RelationModel::GetInstance()->GetRelationId(from_user_id, to_user_id, true);
                        if (session_id != INVALID_VALUE && relate_id != INVALID_VALUE) {
                            msg_id = msg_model->GetMsgId(relate_id);
                            if (msg_id != INVALID_VALUE) {
                                msg_model->SendAudioMessage(relate_id, from_user_id, to_user_id, msg_type, create_time, msg_id, msg.msg_data().c_str(), msg_len);
                                SessionModel::GetInstance()->UpdateSession(session_id, now);
                                SessionModel::GetInstance()->UpdateSession(peer_session_id, now);
                            } else {
                                //log("msgId is invalid. fromId=%u, toId=%u, relate_id=%u, session_id=%u, msg_type=%u", from_user_id, to_user_id, relate_id, session_id, msg_type);
                            }
                        } else {
                            //log("sessionId or relateId is invalid. fromId=%u, toId=%u, relate_id=%u, session_id=%u, msg_type=%u", from_user_id, to_user_id, relate_id, session_id, msg_type);
                        }
                    } else {
                        //log("send msg to self. fromId=%u, toId=%u, msgType=%u", from_user_id, to_user_id, msg_type);
                    }
                }

                //log("fromId=%u, toId=%u, type=%u, msgId=%u, sessionId=%u", from_user_id, to_user_id, msg_type, msg_id, session_id);

                msg.set_msg_id(msg_id);
                //上层数据包给网络库自己复制和不复制两种选择，提高性能
                PacketHandler::SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::MSG_DATA, 
					packet->sequence, &msg, channel_ptr);
            } else {
                //log("msgLen error. fromId=%u, toId=%u, msgType=%u", from_user_id, to_user_id, msg_type);
            }
        } else {
            //log("invalid msgType.fromId=%u, toId=%u, msgType=%u", from_user_id, to_user_id, msg_type);
        }
    } else {
        //log("parse pb failed");
    }
}

void GetMessage(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::GetMsgListRequest msg;
	Im::Message::GetMsgListResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        
        uint32_t user_id = msg.user_id();
        uint32_t peer_id = msg.session_id();
        uint32_t msg_id = msg.msg_id_begin();
        uint32_t msg_cnt = msg.msg_count();
        Im::Base::SessionType session_type = msg.session_type();
        if (Im::Base::SessionType_IsValid(session_type)) {

            std::list<Im::Base::MsgInfo> msg_list;

            if (session_type == Im::Base::SESSION_TYPE_SINGLE) {
            	//获取个人消息
                MessageModel::GetInstance()->GetMessage(user_id, peer_id, msg_id, msg_cnt, msg_list);
            } else if (session_type == Im::Base::SESSION_TYPE_GROUP) {
            	//获取群消息
                if(GroupModel::GetInstance()->IsInGroup(user_id, peer_id))
                {
                    GroupMessageModel::GetInstance()->GetMessage(user_id, peer_id, msg_id, msg_cnt, msg_list);
                }
            }

            response.set_user_id(user_id);
            response.set_session_id(peer_id);
            response.set_msg_id_begin(msg_id);
            response.set_session_type(session_type);
            for (auto it = msg_list.begin(); it != msg_list.end(); it ++) {
                Im::Base::MsgInfo* peer_id = response.add_msg_info_list();
                peer_id->set_msg_id(it->msg_id());
                peer_id->set_from_session_id(it->from_session_id());
                peer_id->set_create_time(it->create_time());
                peer_id->set_msg_type(it->msg_type());
                peer_id->set_msg_data(it->msg_data());
            }

            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_MSG_LIST, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("invalid session_type. userId=%u, peerId=%u, msgId=%u, msgCnt=%u, sessiosession_type=%u",user_id, peer_id, msg_id, msg_cnt, nSessiosession_type);
        }
        
    } else {

    }
}

void GetMsgById(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::GetMsgByIdRequest msg;
	Im::Message::GetMsgByIdResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t user_id = msg.user_id();
        Im::Base::SessionType session_type = msg.session_type();
        uint32_t peer_id = msg.session_id();
        std::list<uint32_t> msg_id_list;
        uint32_t total = msg.msg_id_list_size();
        for (uint32_t i = 0; i < total; i ++) {
            msg_id_list.push_back(msg.msg_id_list(i));
        }
        if (Im::Base::SessionType_IsValid(session_type)) {

            std::list<Im::Base::MsgInfo> msg_info_list;
            if (Im::Base::SESSION_TYPE_SINGLE == session_type) {
                MessageModel::GetInstance()->GetMsgByMsgId(user_id, peer_id, msg_id_list, msg_info_list);
            } else if(Im::Base::SESSION_TYPE_GROUP) {
                GroupMessageModel::GetInstance()->GetMsgByMsgId(user_id, peer_id, msg_id_list, msg_info_list);
            }
            response.set_user_id(user_id);
            response.set_session_id(peer_id);
            response.set_session_type(session_type);
            for (auto it = msg_info_list.begin(); it != msg_info_list.end(); it ++) {
                Im::Base::MsgInfo* peer_id = response.add_msg_info_list();
                peer_id->set_msg_id(it->msg_id());
                peer_id->set_from_session_id(it->from_session_id());
                peer_id->set_create_time(it->create_time());
                peer_id->set_msg_type(it->msg_type());
                peer_id->set_msg_data(it->msg_data());
            }
            //log("userId=%u, peerId=%u, sessiosession_type=%u, reqMsgCnt=%u, resMsgCnt=%u", user_id, peer_id, session_type, msg.msg_id_list_size(), response.msg_list_size());
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_MSG_BY_MSG_ID, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("invalid sessiosession_type. fromId=%u, toId=%u, sessiosession_type=%u, msgCnt=%u", user_id, peer_id, session_type, nCnt);
        }
    } else {
        //log("parse pb failed");
    }
}

void GetLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::GetLatestMsgIdResquest msg;
	Im::Message::GetLatestMsgIdResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t user_id = msg.user_id();
        Im::Base::SessionType session_type = msg.session_type();
        uint32_t peer_id = msg.session_id();
        if (Im::Base::SessionType_IsValid(session_type)) {
          
            response.set_user_id(peer_id);
            response.set_session_type(session_type);
            response.set_session_id(peer_id);
            uint32_t msg_id = INVALID_VALUE;
            if (Im::Base::SESSION_TYPE_SINGLE == session_type) {
                std::string str_msg;
                Im::Base::MsgType msg_type;
                MessageModel::GetInstance()->GetLastMsg(peer_id, peer_id, msg_id, str_msg, msg_type, 1);
            } else {
                std::string str_msg;
                Im::Base::MsgType msg_type;
                uint32_t from_id = INVALID_VALUE;
                GroupMessageModel::GetInstance()->GetLastMsg(peer_id, msg_id, str_msg, msg_type, from_id);
            }
            response.set_latest_msg_id(msg_id);
            //log("userId=%u, peerId=%u, sessionType=%u, msgId=%u", peer_id, peer_id, session_type,msg_id);
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_LATEST_MSG_ID, 
				packet->sequence, &response, channel_ptr);
        } else {
            //log("invalid sessionType. userId=%u, peerId=%u, sessionType=%u", peer_id, peer_id, session_type);
        }
    } else {
        //log("parse pb failed");
    }
}

void GetUnreadMsgCounter(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::UnreadMsgCntRequest msg;
	Im::Message::UnreadMsgCntResponse response;
	if (msg.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = msg.user_id();
		std::list<Im::Base::UnreadInfo> unread_info_list;
		uint32_t total = 0;
		MessageModel::GetInstance()->GetUnreadMsgCnt(user_id, total, unread_info_list);
		GroupMessageModel::GetInstance()->GetUnreadMsgCnt(user_id, total, unread_info_list);

		response.set_user_id(user_id);
		response.set_total_count(total);

		for (auto it = unread_info_list.begin(); it != unread_info_list.end(); it ++) {
			Im::Base::UnreadInfo* unread_info = response.add_unread_info_list();
			unread_info->set_session_id(it->session_id());
			unread_info->set_session_type(it->session_type());
			unread_info->set_unread_count(it->unread_count());
			unread_info->set_latest_msg_id(it->latest_msg_id());
			unread_info->set_latest_msg_data(it->latest_msg_data());
			unread_info->set_latest_msg_type(it->latest_msg_type());
			unread_info->set_latest_msg_from_user_id(it->latest_msg_from_user_id());
		}

		response.set_attach_data(msg.attach_data());

		PacketHandler::SendMessage(Im::Base::SERVICE_MESSAGE, Im::Base::RES_UNREAD_COUNT, 
			packet->sequence, &response, channel_ptr);
	} else {

	}
}

void ClearUnreadMsgCounter(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Message::MessageDataReadAck msg;
	if (msg.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = msg.user_id();
		uint32_t session_id = msg.session_id();
		Im::Base::SessionType session_type = msg.session_type();
		UserModel::GetInstance()->ClearUserCounter(user_id, session_id, session_type);
	} else {

	}
}

void SetDeviceToken(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Login::DeviceTokenRequest msg;
	Im::Login::DeviceTokenResponse response;
	if (msg.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = msg.user_id();
		std::string token = msg.device_token();

        CacheManager* cache_manager = CacheManager::GetInstance();
		if (cache_manager == nullptr) {
			//WARN << "cache_manager is nullptr";
			return;
		}
		CacheConnector* cache_connector = cache_manager->GetCacheConnector("token");
		if (cache_connector == nullptr) {
			//WARN << "cache_connector is nullptr";
		} else {
			Im::Base::ClientType client_type = msg.client_type();
            std::string str_token;
            if (Im::Base::CLIENT_TYPE_IOS == client_type) {
                str_token = "ios:" + token;
            } else if(Im::Base::CLIENT_TYPE_ANDROID == client_type) {
                str_token = "android:" + token;
            } else {
                str_token = token;
            }
            
            std::string str_old_token = cache_connector->Get("device_" + Util::Uint32ToString(user_id));
            
            if(!str_old_token.empty())
            {
                size_t pos = str_old_token.find(":");
                if(pos != std::string::npos)
                {
                    std::string str_old = str_old_token.substr(pos + 1);
                    std::string str_reply = cache_connector->Get("device_" + str_old);
                    if (!str_reply.empty()) {
                        std::string str_new_value("");
                        cache_connector->Set("device_" + str_old, str_new_value);
                    }
                }
            }
            
            cache_connector->Set("device_" + Util::Uint32ToString(user_id), str_token);
            std::string str_new_value = Util::Uint32ToString(user_id);
            cache_connector->Set("device_" + token, str_new_value);
        
            cache_manager->ReleaseCacheConnector(cache_connector);
		}
        response.set_user_id(user_id);
        response.set_attach_data(msg.attach_data());

        PacketHandler::SendMessage(Im::Base::SERVICE_LOGIN, Im::Base::RES_DEVICE_TOKEN, 
			packet->sequence, &response, channel_ptr);

	} else {

	}
}

void GetDeviceToken(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Server::GetDeviceTokenRequest msg;
    Im::Server::GetDeviceTokenResponse response;
    if(msg.ParseFromArray(packet->data, packet->length - 18)) {
    	uint32_t total = msg.user_id_size();
        CacheManager* cache_manager = CacheManager::GetInstance();
		if (cache_manager == nullptr) {
			//WARN << "cache_manager is nullptr";
			return;
		}
		CacheConnector* cache_connector = cache_manager->GetCacheConnector("token");
		if (cache_connector == nullptr) {
			//WARN << "cache_connector is nullptr";
		} else {
			std::vector<std::string> token_vec;
            for (uint32_t i = 0; i < total; i ++) {
                std::string str_key = "device_" + Util::Uint32ToString(msg.user_id(i));
                token_vec.push_back(str_key);
            }
            std::map<std::string, std::string> token_map;
            bool flag = cache_connector->Mget(token_vec, token_map);
            cache_manager->ReleaseCacheConnector(cache_connector);
            
            if (flag) {
                for (auto it = token_map.begin(); it != token_map.end(); it ++) {
                    std::string str_key = it->first;
                    size_t pos = str_key.find("device_");
                    if (pos != std::string::npos) {
                        std::string str_user_id = str_key.substr(pos + strlen("device_"));
                        uint32_t user_id = Util::StringToUint32(str_user_id);
                        std::string str_value = it->second;
                        pos = str_value.find(":");
                        if (pos != std::string::npos) {
                            std::string str_type = str_value.substr(0, pos);
                            std::string str_token = str_value.substr(pos + 1);
                            Im::Base::ClientType client_type = Im::Base::ClientType(0);
                            if (str_type == "ios") {
                                client_type = Im::Base::CLIENT_TYPE_IOS;
                            } else if (str_type == "android") {
                                client_type = Im::Base::CLIENT_TYPE_ANDROID;
                            }
                            if (Im::Base::ClientType_IsValid(client_type)) {
                                Im::Base::UserTokenInfo* token_info = response.add_user_token_info();
                                token_info->set_user_id(user_id);
                                token_info->set_token(str_token);
                                token_info->set_client_type(client_type);
                                uint32_t total = 0;
                                MessageModel::GetInstance()->GetUnreadCntAll(user_id, total);
                                GroupMessageModel::GetInstance()->GetUnreadCntAll(user_id, total);
                                token_info->set_push_count(total);
                                token_info->set_push_type(1);
                            }
                            else
                            {
                                //DEBUG << "invalid clientType.clientType=" + client_type;
                            }
                        }
                        else
                        {
                            //DEBUG << "invalid value. value=" + strValue.c_str();
                        }
                        
                    } else {
                    	//DEBUG << "invalid key.key=" + strKey.c_str();
                    }
                }
            } else {

            }
		}
        
        response.set_attach_data(msg.attach_data());
        PacketHandler::SendMessage(Im::Base::SERVICE_OTHER, Im::Base::RES_GET_DEVICE_TOKEN, 
			packet->sequence, &response, channel_ptr);
    } else {
    	
    }
}

} // namespace Action

} // namespace marz
