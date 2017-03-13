#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include "protobuf/Im.Base.pb.h"
using namespace Im;

#include <list>
#include <map>
#include "Define.h"

class UserModel {
public:
	virtual ~UserModel();
	static UserModel *getInstance();

	void getChangedId(uint32_t& last_time, std::list<uint32_t>& id_list);
	void getUserList(std::list<uint32_t> user_id_list, std::list<Im::Base::UserInfo>& user_info_list);
	bool getUser(uint32_t user_id, mars::user_info_t& user);
	bool updateUser(mars::user_info_t& user);
	bool insertUser(mars::user_info_t& user);
	void clearUserCounter(uint32_t user_id, uint32_t peer_id, Im::Base::SessionType session_type);

private:
	UserModel();

	static UserModel *instance_;
};

#endif // __USER_MODLE_H__