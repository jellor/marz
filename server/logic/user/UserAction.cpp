#include "Im.Buddy.pb.h"
#include "UserAction.h"
#include "UserModel.h"
#include <list>

namespace marz {

namespace Action {

void GetUserInfo(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Buddy::UserInfoRequest msg;
	Im::Buddy::UserInfoResponse response;
	if (msg.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t from_user_id = msg.user_id();
		uint32_t user_count = msg.user_id_list_size();
		std::list<uint32_t> id_list;
		for (uint32_t i = 0; i < user_count; i ++) {
			id_list.push_back(msg.user_id_list(i));
		}
		std::list<Im::Base::UserInfo> user_list;
		UserModel::GetInstance()->GetUserList(id_list, user_list);
		response.set_user_id(from_user_id);
		for (auto cur_user = user_list.begin(); cur_user != user_list.end(); cur_user ++) {
			Im::Base::UserInfo* user_info = response.add_user_info_list();
			user_info->set_user_id(cur_user->user_id());
			user_info->set_sex(cur_user->sex());
			user_info->set_nick(cur_user->nick());
			user_info->set_avatar(cur_user->avatar());
			user_info->set_phone(cur_user->phone());
			user_info->set_email(cur_user->email());
			user_info->set_name(cur_user->name());
			user_info->set_domain(cur_user->domain());
			user_info->set_status(cur_user->status());
		}

		response.set_attach_data(msg.attach_data());
		PacketHandler::SendMessage(Im::Base::SERVICE_BUDDY_LIST, Im::Base::RES_USER_INFO,
			packet->sequence, &response, channel_ptr);
	} else {

	}
}

void GetChangeUser(const ChannelPtr& channel_ptr, Packet* packet)
{
	Im::Buddy::AllUserRequest msg;
    Im::Buddy::AllUserResponse response;
	if (msg.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = msg.user_id();
		uint32_t last_time = msg.latest_update_time();
		
		std::list<Im::Base::UserInfo> user_list;
		std::list<uint32_t> id_list;
		UserModel::GetInstance()->GetChangedId(last_time, id_list);
		UserModel::GetInstance()->GetUserList(id_list, user_list);

		response.set_user_id(user_id);
		response.set_latest_update_time(last_time);

		for (auto cur_user = user_list.begin(); cur_user != user_list.end(); cur_user ++) {
			Im::Base::UserInfo* user_info = response.add_user_info_list();
			user_info->set_user_id(cur_user->user_id());
			user_info->set_sex(cur_user->sex());
			user_info->set_nick(cur_user->nick());
			user_info->set_avatar(cur_user->avatar());
			user_info->set_phone(cur_user->phone());
			user_info->set_email(cur_user->email());
			user_info->set_name(cur_user->name());
			user_info->set_domain(cur_user->domain());
			user_info->set_status(cur_user->status());
		}

		response.set_attach_data(msg.attach_data());
		PacketHandler::SendMessage(Im::Base::SERVICE_BUDDY_LIST, Im::Base::RES_ALL_USER,
			packet->sequence, &response, channel_ptr);
	} else {
		
	}
}

} // namespace Action

} // namespace marz
