#ifndef __GROUP_MODEL_H__
#define __GROUP_MODEL_H__

#include "Im.Base.pb.h"

#include <list>
#include <set>
#include <map>

namespace marz {

class GroupModel {
public:
	virtual ~GroupModel();
	static GroupModel* GetInstance();

	uint32_t CreateGroup(uint32_t user_id, const std::string& group_name, const std::string& group_avatar, uint32_t group_type, std::set<uint32_t>& member_set);
	bool RemoveGroup(uint32_t user_id, uint32_t group_id, std::list<uint32_t> cur_user_id_list);
	void GetUserGroup(uint32_t user_id, std::list<Im::Base::GroupVersionInfo>& group_list, uint32_t group_type);
	void GetUserGroupIdList(uint32_t user_id, std::list<uint32_t>& group_id_list, uint32_t limit = 100);
	void GetGroupInfo(std::map<uint32_t, Im::Base::GroupVersionInfo>& group_id_map, std::list<Im::Base::GroupInfo>& group_info_list);
	bool SetPush(uint32_t user_id, uint32_t group_id, uint32_t type, uint32_t status);
	void GetPush(uint32_t group_id, std::list<uint32_t>& user_list, std::list<Im::Base::ShieldStatus>& push_list);
	bool ModifyGroupMember(uint32_t user_id, uint32_t group_id, Im::Base::GroupModifyType type, std::set<uint32_t>& user_id_set, std::list<uint32_t>& cur_user_id_list);
	void GetGroupUser(uint32_t group_id, std::list<uint32_t>& user_id_list);
	bool IsInGroup(uint32_t user_id, uint32_t group_id);
	void UpdateGroupChat(uint32_t group_id);
	bool IsInvalidateGroupId(uint32_t group_id);
	uint32_t GetUserJoinTime(uint32_t group_id, uint32_t user_id);

private:
	GroupModel();

	bool InsertNewGroup(uint32_t user_id, const std::string& group_name, const std::string& group_avatar, uint32_t group_type, uint32_t member_cnt, uint32_t& group_id);
	bool InsertNewMember(uint32_t group_id, std::set<uint32_t>& user_set);
	std::string GenerateGroupAvatar(uint32_t group_id);
	void GetGroupVersion(std::list<uint32_t>& group_id_list, std::list<Im::Base::GroupVersionInfo>& group_list, uint32_t group_type);
	bool HasModifyPermission(uint32_t user_id, uint32_t group_id, Im::Base::GroupModifyType type);
	bool AddMember(uint32_t group_id, std::set<uint32_t>& user_set, std::list<uint32_t>& cur_user_id_list);
	bool RemoveMember(uint32_t group_id, std::set<uint32_t>& user_set, std::list<uint32_t>& cur_user_id_list);
	void RemoveRepeatUser(uint32_t group_id, std::set<uint32_t>& user_set);
	void RemoveSession(uint32_t group_id, const std::set<uint32_t>& user_set);
	bool IncGroupVersion(uint32_t group_id);
	void ClearGroupMember(uint32_t group_id);
	void FillGroupMember(std::list<Im::Base::GroupInfo>& group_list);

	static GroupModel *instance_;

};

} // namespace marz

#endif // __GROUP_MODEL_H__
