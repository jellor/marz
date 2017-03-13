#ifndef __GROUP_MESSAGE_MODEL_H__
#define __GROUP_MESSAGE_MODEL_H__

#include <list>
#include "Im.Base.pb.h"

namespace marz {

class GroupMessageModel {
public:
	virtual ~GroupMessageModel();
	static GroupMessageModel* GetInstance();

	bool SendMessage(uint32_t from_id, uint32_t group_id, Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, const std::string& msg_content);
	bool SendAudioMessage(uint32_t from_id, uint32_t group_id, Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, const char *msg_content, uint32_t msg_len);
	void GetMessage(uint32_t user_id, uint32_t groud_id, uint32_t msg_id, uint32_t msg_cnt, std::list<Im::Base::MsgInfo>& msg_info_list);
	bool ClearMsgCnt(uint32_t user_id, uint32_t group_id);
	uint32_t GetMsgId(uint32_t group_id);
	void GetUnreadMsgCnt(uint32_t user_id, uint32_t& total_cnt, std::list<Im::Base::UnreadInfo>& unread_cnt_list);
	void GetLastMsg(uint32_t group_id, uint32_t& msg_id, std::string& msg_data, Im::Base::MsgType& msg_type, uint32_t& from_id);
	void GetUnreadCntAll(uint32_t user_id, uint32_t& total_cnt);
	void GetMsgByMsgId(uint32_t user_id, uint32_t group_id, const std::list<uint32_t>& msg_id_list, std::list<Im::Base::MsgInfo>& msg_info_list);
	bool ResetMsgId(uint32_t group_id);

private:
	GroupMessageModel();
	bool IncMsgCnt(uint32_t user_id, uint32_t group_id);

	static GroupMessageModel *instance_;

};

} // namespace marz

#endif // __GROUP_MESSAGE_MODEL_H__