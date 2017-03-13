#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include "Im.Base.pb.h"

#include <list>
#include <map>
#include "Types.h"

namespace marz {

class UserModel {
public:
	virtual ~UserModel();
	static UserModel *GetInstance();

	void GetChangedId(uint32_t& last_time, std::list<uint32_t>& id_list);
	void GetUserList(std::list<uint32_t> user_id_list, std::list<Im::Base::UserInfo>& user_info_list);
	bool GetUser(uint32_t user_id, user_info_t& user);
	bool UpdateUser(user_info_t& user);
	bool InsertUser(user_info_t& user);
	void ClearUserCounter(uint32_t user_id, uint32_t peer_id, Im::Base::SessionType session_type);

private:
	UserModel();

	static UserModel *instance_;
	
};

} // namespace marz

#endif // __USER_MODLE_H__
