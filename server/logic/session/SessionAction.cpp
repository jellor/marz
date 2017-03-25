#include <list>
#include "Im.Session.pb.h"
#include "SessionAction.h"
#include "SessionModel.h"
#include "UserModel.h"
#include "Types.h"

namespace marz {

namespace Action {

void GetRecentSession(const ChannelPtr& channel_ptr, Packet *packet) {

	Im::Session::RecentContactSessionRequest request;
	Im::Session::RecentContactSessionResponse response;

	if (request.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = request.user_id();
		uint32_t last_time = request.latest_update_time();

		std::list<Im::Base::ContactSessionInfo> contact_list;
		SessionModel::GetInstance()->GetRecentSession(user_id, last_time, contact_list);

		response.set_user_id(user_id);

		for (auto it = contact_list.begin(); it != contact_list.end(); it ++) {
			Im::Base::ContactSessionInfo* contact = response.add_contact_session_list();
			contact->set_session_id(it->session_id());
			contact->set_session_type(it->session_type());
			contact->set_session_status(it->session_status());
			contact->set_update_time(it->update_time());
			contact->set_latest_msg_id(it->latest_msg_id());
			contact->set_latest_msg_data(it->latest_msg_data());
			contact->set_latest_msg_type(it->latest_msg_type());
			contact->set_latest_msg_from_user_id(it->latest_msg_from_user_id());
		}

		response.set_attach_data(request.attach_data());

		PacketHandler::SendMessage(Im::Base::SERVICE_BUDDY_LIST, Im::Base::RES_RECENT_CONTACT_SESSION, 
			packet->sequence, &response, channel_ptr);

	}
}

void DelRecentSession(const ChannelPtr& channel_ptr, Packet *packet) {

	Im::Session::RemoveSessionRequest request;
	Im::Session::RemoveSessionResponse response;

	if (request.ParseFromArray(packet->data, packet->length - 18)) {
		uint32_t user_id = request.user_id();
		uint32_t peer_id = request.session_id();
		Im::Base::SessionType session_type = request.session_type();
		if (Im::Base::SessionType_IsValid(session_type)) {
			bool ret = false;
			uint32_t session_id = SessionModel::GetInstance()->GetSessionId(user_id, peer_id, session_type, false);
			if (session_id != INVALID_VALUE) {
				ret = SessionModel::GetInstance()->RemoveSession(session_id);
				if (ret) {
					UserModel::GetInstance()->ClearUserCounter(user_id, peer_id, session_type);
				}
				response.set_user_id(user_id);
				response.set_result_code(ret ? 0 : 1);
				response.set_session_type(session_type);
				response.set_session_id(session_id);
				response.set_attach_data(request.attach_data());
				PacketHandler::SendMessage(Im::Base::SERVICE_BUDDY_LIST, Im::Base::RES_REMOVE_SESSION, 
					packet->sequence, &response, channel_ptr);
			} else {

			}
		} else {

		}
	}

}

} // namespace Action

} // namespace marz
