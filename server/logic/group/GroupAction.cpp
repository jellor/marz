#include <set>
#include <list>
#include <map>
#include "Im.Group.pb.h"
#include "Im.Server.pb.h"
#include "GroupAction.h"
#include "GroupModel.h"
#include "Types.h"

namespace marz {

namespace Action {

void CreateGroup(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Group::GroupCreateRequest msg;
    Im::Group::GroupCreateResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
       
        uint32_t user_id = msg.user_id();
        std::string group_name = msg.group_name();
        Im::Base::GroupType group_type = msg.group_type();
        if (Im::Base::GroupType_IsValid(group_type)) {
            std::string group_avatar = msg.group_avatar();
            std::set<uint32_t> member_set;
            uint32_t total = msg.group_member_id_list_size();
            for (uint32_t i = 0; i < total; i ++) {
                uint32_t user_id = msg.group_member_id_list(i);
                member_set.insert(user_id);
            }
            //log("createGroup.%d create %s, userCnt=%u", user_id, group_name.c_str(), member_set.size());
            
            uint32_t group_id = GroupModel::GetInstance()->CreateGroup(user_id, group_name, group_avatar, group_type, member_set);
            response.set_user_id(user_id);
            response.set_group_name(group_name);
            for (auto it = member_set.begin(); it != member_set.end(); it ++) {
                response.add_user_id_list(*it);
            }
            if (group_id != INVALID_VALUE) {
                response.set_result_code(0);
                response.set_group_id(group_id);
            } else {
                response.set_result_code(1);
            }
            
            
            //log("createGroup.%d create %s, userCnt=%u, result:%d", user_id, group_name.c_str(), member_set.size(), response.result_code());
            
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_GROUP, Im::Base::RES_CREATE_GROUP, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("invalid group type.userId=%u, groupType=%u, groupName=%s", user_id, group_type, group_name.c_str());
        }
    } else {
        //log("parse pb failed");
    }
}

void GetNormalGroupList(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Group::NormalGroupListRequest msg;
    Im::Group::NormalGroupListResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        
        uint32_t user_id = msg.user_id();
        
        std::list<Im::Base::GroupVersionInfo> group_version_info_list;
        GroupModel::GetInstance()->GetUserGroup(user_id, group_version_info_list, Im::Base::GROUP_TYPE_NORMAL);
        response.set_user_id(user_id);
        for (auto it = group_version_info_list.begin(); it != group_version_info_list.end(); it ++) {
            Im::Base::GroupVersionInfo* group_version_info = response.add_group_version_list();
            group_version_info->set_group_id(it->group_id());
            group_version_info->set_version(it->version());
        }
        
        //log("getNormalGroupList. userId=%u, count=%d", user_id, response.group_version_list_size());
        
        response.set_attach_data(msg.attach_data());
        PacketHandler::SendMessage(Im::Base::SERVICE_GROUP, Im::Base::RES_NORMAL_LIST, 
					packet->sequence, &response, channel_ptr);
    } else {
        //log("parse pb failed");
    }
}

void GetGroupInfo(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Group::GroupInfoListRequest msg;
    Im::Group::GroupInfoListResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
   
        uint32_t user_id = msg.user_id();
        uint32_t total = msg.group_version_list_size();
        
        std::map<uint32_t, Im::Base::GroupVersionInfo> group_id_map;
        for (uint32_t i = 0; i < total; i ++) {
            Im::Base::GroupVersionInfo group_version_info = msg.group_version_list(i);
            if (GroupModel::GetInstance()->IsInvalidateGroupId(group_version_info.group_id())) {
                group_id_map[group_version_info.group_id()] = group_version_info;
            }
        }
        std::list<Im::Base::GroupInfo> group_info_list;
        GroupModel::GetInstance()->GetGroupInfo(group_id_map, group_info_list);
        
        response.set_user_id(user_id);
        for (auto it= group_info_list.begin(); it != group_info_list.end(); it ++) {
            Im::Base::GroupInfo* group_info = response.add_group_info_list();
//            *group_info = *it;
            group_info->set_group_id(it->group_id());
            group_info->set_version(it->version());
            group_info->set_group_name(it->group_name());
            group_info->set_group_avatar(it->group_avatar());
            group_info->set_from_user_id(it->from_user_id());
            group_info->set_group_type(it->group_type());
            group_info->set_shield_status(it->shield_status());
            uint32_t total = it->group_member_list_size();
            for (uint32_t i = 0; i < total; i ++) {
                uint32_t user_id = it->group_member_list(i);
                group_info->add_group_member_list(user_id);
            }
        }
        
        //log("userId=%u, requestCount=%u", user_id, nGroupCnt);
        
        response.set_attach_data(msg.attach_data());
        PacketHandler::SendMessage(Im::Base::SERVICE_GROUP, Im::Base::RES_GROUP_INFO, 
					packet->sequence, &response, channel_ptr);
    } else {
        //log("parse pb failed");
    }
}

void ModifyMember(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Group::GroupChangeMemberRequest msg;
    Im::Group::GroupChangeMemberResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t user_id = msg.user_id();
        uint32_t group_id = msg.group_id();
        Im::Base::GroupModifyType group_modify_type = msg.change_type();
        if (Im::Base::GroupModifyType_IsValid(group_modify_type) &&
            GroupModel::GetInstance()->IsInvalidateGroupId(group_id)) {
            
            uint32_t total = msg.member_id_list_size();
            std::set<uint32_t> user_id_set;
            for (uint32_t i = 0; i < total; i ++) {
                user_id_set.insert(msg.member_id_list(i));
            }
            std::list<uint32_t> cur_user_id_list;
            bool flag = GroupModel::GetInstance()->ModifyGroupMember(user_id, group_id, group_modify_type, user_id_set, cur_user_id_list);
            response.set_user_id(user_id);
            response.set_group_id(group_id);
            response.set_change_type(group_modify_type);
            response.set_result_code(flag?0:1);
            if (flag) {
                for (auto it = user_id_set.begin(); it != user_id_set.end(); it ++) {
                    response.add_change_user_id_list(*it);
                }
                
                for (auto it = cur_user_id_list.begin(); it != cur_user_id_list.end(); it ++) {
                    response.add_current_user_id_list(*it);
                }
            }
            //log("userId=%u, groupId=%u, result=%u, changeCount:%u, currentCount=%u",user_id, group_id,  flag?0:1, response.chg_user_id_list_size(), response.cur_user_id_list_size());
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_GROUP, Im::Base::RES_CHANGE_GROUP_MEMBER, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("invalid groupModifyType or groupId. userId=%u, groupId=%u, groupModifyType=%u", user_id, group_id, group_modify_type);
        }
        
    } else {
        //log("parse pb failed");
    }
}

void SetGroupPush(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Group::GroupShieldRequest msg;
    Im::Group::GroupShieldResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t user_id = msg.user_id();
        uint32_t group_id = msg.group_id();
        uint32_t shield_status = msg.shield_status();
        if (GroupModel::GetInstance()->IsInvalidateGroupId(group_id)) {
           
            bool flag = GroupModel::GetInstance()->SetPush(user_id, group_id, IM_GROUP_SETTING_PUSH, shield_status);
            
            response.set_user_id(user_id);
            response.set_group_id(group_id);
            response.set_result_code(flag?0:1);
        
            //log("userId=%u, groupId=%u, result=%u", user_id, group_id, response.result_code());
            
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_GROUP, Im::Base::RES_SHIELD_GROUP, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("Invalid group.userId=%u, groupId=%u", user_id, group_id);
        }
    } else {
        //log("parse pb failed");
    }
}

void GetGroupPush(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Server::GroupGetShieldRequest msg;
    Im::Server::GroupGetShieldResponse response;
    if (msg.ParseFromArray(packet->data, packet->length - 18)) {
        uint32_t group_id = msg.group_id();
        uint32_t total = msg.user_id_size();
        if (GroupModel::GetInstance()->IsInvalidateGroupId(group_id)) {
            
            std::list<uint32_t> user_id_list;
            for (uint32_t i = 0; i < total; i ++) {
                user_id_list.push_back(msg.user_id(i));
            }
            std::list<Im::Base::ShieldStatus> push_list;
            GroupModel::GetInstance()->GetPush(group_id, user_id_list, push_list);
            
            response.set_group_id(group_id);
            for (auto it = push_list.begin(); it != push_list.end(); it ++) {
                Im::Base::ShieldStatus* shield_status = response.add_shield_status_list();
    //            *shield_status = *it;
                shield_status->set_user_id(it->user_id());
                shield_status->set_group_id(it->group_id());
                shield_status->set_shield_status(it->shield_status());
            }
            
            //log("groupId=%u, count=%u", group_id, nUserCnt);
            response.set_attach_data(msg.attach_data());
            PacketHandler::SendMessage(Im::Base::SERVICE_OTHER, Im::Base::RES_GET_SHIELD, 
					packet->sequence, &response, channel_ptr);
        } else {
            //log("Invalid groupId. group_id=%u", group_id);
        }
    } else {
        //log("parse pb failed");
    }
}

} // namespace Action

} // namespace marz
