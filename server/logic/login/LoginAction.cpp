#include "Im.Login.pb.h"
#include "LoginAction.h"
#include "LoginModel.h"

namespace marz {

namespace Action {

void Login(const ChannelPtr& channel_ptr, Packet* packet) {

	Im::Login::ValidateRequest request;
	Im::Login::ValidateResponse response;

	if (request.ParseFromArray(packet->data, packet->length - 18)) {

		std::string user_name = request.user_name();
		std::string user_password = request.user_password();

		response.set_user_name(user_name);
		response.set_attach_data(request.attach_data());

		Im::Base::UserInfo cur_user;
		if (LoginModel::DoLogin(user_name, user_password, cur_user)) {
			Im::Base::UserInfo* user_ptr = response.mutable_user_info();
			user_ptr->set_user_id(cur_user.user_id());
			user_ptr->set_sex(cur_user.sex());
			user_ptr->set_nick(cur_user.nick());
			user_ptr->set_avatar(cur_user.avatar());
			user_ptr->set_phone(cur_user.phone());
			user_ptr->set_email(cur_user.email());
			user_ptr->set_name(cur_user.name());
			user_ptr->set_domain(cur_user.domain());
			user_ptr->set_status(0);
			response.set_result_code(0);
			response.set_result_string("成功");

		} else {
			response.set_result_code(1);
			response.set_result_string("用户/密码错误");
		}

	} else {
		response.set_result_code(2);
		response.set_result_string("服务器内部问题");
	}

	PacketHandler::SendMessage(Im::Base::SERVICE_OTHER, Im::Base::RES_VALIDATE, packet->sequence, &response, channel_ptr);

	delete packet;
}

} // namespace Action

} // namespace marz
