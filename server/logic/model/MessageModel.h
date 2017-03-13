#ifndef __MESSAGE_MODEL_H__
#define __MESSAGE_MODEL_H__

#include "Im.Base.pb.h"

#include <list>
#include <map>

namespace marz {

class MessageModel {
public:
	~MessageModel();
	static MessageModel *GetInstance();

	bool SendMessage(uint32_t relate_id, uint32_t from_id, uint32_t to_id, Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, std::string& msg_content);  
	bool SendAudioMessage(uint32_t relate_id, uint32_t from_id, uint32_t to_id, Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, const char *msg_content, uint32_t msg_len);   
	void GetMessage(uint32_t user_id, uint32_t peer_id, uint32_t msg_id, uint32_t msg_cnt, std::list<Im::Base::MsgInfo>& msg_info_list);
	bool ClearMsgCnt(uint32_t user_id, uint32_t peer_id);
	uint32_t GetMsgId(uint32_t relate_id);
	void GetUnreadMsgCnt(uint32_t user_id, uint32_t& total_cnt, std::list<Im::Base::UnreadInfo>& unread_info_list);
	void GetLastMsg(uint32_t from_id, uint32_t to_id, uint32_t& msg_id, std::string& msg_content, Im::Base::MsgType& msg_type, uint32_t status = 0);
	void GetUnreadCntAll(uint32_t user_id, uint32_t& total_cnt);
	void GetMsgByMsgId(uint32_t user_id, uint32_t peer_id, const std::list<uint32_t>& msg_id_list, std::list<Im::Base::MsgInfo>& msg_info_list);
	bool ResetMsgId(uint32_t relate_id);

private:
	MessageModel();
	void IncMsgCnt(uint32_t from_id, uint32_t to_id);

	static MessageModel *instance_;

};

} // namespace marz

#endif // __MESSAGE_MODEL_H__
