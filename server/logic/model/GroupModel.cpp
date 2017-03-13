#include "GroupModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "GroupMessageModel.h"
#include "SessionModel.h"
#include "UserModel.h"
#include "Util.h"

namespace marz {

GroupModel* GroupModel::instance_ = NULL;

GroupModel::GroupModel()
{	
}

GroupModel::~GroupModel() {
}

GroupModel* GroupModel::GetInstance() {
	
	if (instance_ == NULL) {
		instance_ = new GroupModel();
	}

	return instance_;
}

uint32_t GroupModel::CreateGroup(uint32_t user_id, const std::string& group_name, 
	const std::string& group_avatar, uint32_t group_type, std::set<uint32_t>& member_set) {
	
	uint32_t group_id = INVALID_VALUE;
	do {
		if (group_name.empty()) {
			break;
		}
		if (member_set.empty()) {
			break;
		}

		if (!InsertNewGroup(user_id, group_name, group_avatar, group_type, (uint32_t)member_set.size(), group_id)) {
			break;
		}

		bool ret = GroupMessageModel::GetInstance()->ResetMsgId(group_id);
		if (!ret) {
			WLOG << "reset msg id failed";
		}
		ClearGroupMember(group_id);
		InsertNewMember(group_id, member_set);
	} while (false);

	return group_id;
}

bool GroupModel::RemoveGroup(uint32_t user_id, uint32_t group_id, std::list<uint32_t> cur_user_id_list) {

	bool ret = false;
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}
	std::set<uint32_t> user_id_set;

	std::string sql = "SELECT creator FROM ImGroup WHERE id=" + Util::Uint32ToString(group_id);
	ResultSet *result = db_connector->Query(sql);
	if (result == NULL) {
		WLOG << sql << " failed";
	} else {
		uint32_t create_id;
		while (result->Next()) {
			create_id = result->GetInt("creator");
		}
		delete result;
		if (create_id == 0 || create_id == user_id) {
			sql = "UPDATE ImGroup SET status=0 WHERE id=" + Util::Uint32ToString(group_id);
			ret = db_connector->Update(sql);
		}
	}

	if (ret) {
		sql = "SELECT user_id FROM ImGroupMember WHERE group_id=" + Util::Uint32ToString(group_id);
		result = db_connector->Query(sql);
		if (result == NULL) {
			WLOG << sql << " failed";
		} else {
			while (result->Next()) {
				uint32_t id = result->GetInt("user_id");
				user_id_set.insert(id);
			}
			delete result;
		}
	}	

	db_manager->ReleaseDatabaseConnector(db_connector);
	if (ret) {
		ret = RemoveMember(group_id, user_id_set, cur_user_id_list);
	}

	return ret;
}

void GroupModel::GetUserGroup(uint32_t user_id, std::list<Im::Base::GroupVersionInfo>& group_list, uint32_t group_type) {
	std::list<uint32_t> group_id_list;
	GetUserGroupIdList(user_id, group_id_list);
	if (!group_id_list.empty()) {
		GetGroupVersion(group_id_list, group_list, group_type);
	}
}

void GroupModel::GetUserGroupIdList(uint32_t user_id, std::list<uint32_t>& group_id_list, uint32_t limit) {
	
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return;
	}

	std::string sql = "SELECT group_id FROM ImGroupMember WHERE user_id=" + Util::Uint32ToString(user_id) + " AND status=0 ORDER BY updated DESC, id DESC";
	if (limit != 0) {
		sql += " LIMIT " + Util::Uint32ToString(limit);
	}

	ResultSet *result = db_connector->Query(sql);
	if (result == NULL) {
		DLOG << sql << " failed";
	} else {
		Im::Base::GroupVersionInfo group;
		while (result->Next()) {
			uint32_t group_id = result->GetInt("group_id");
			group_id_list.push_back(group_id);
		}
		delete result;
	}

	db_manager->ReleaseDatabaseConnector(db_connector);
}

void GroupModel::GetGroupInfo(std::map<uint32_t, Im::Base::GroupVersionInfo>& group_id_map, 
	std::list<Im::Base::GroupInfo>& group_info_list) {
	
	if (!group_id_map.empty()) {
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == nullptr) {
			WLOG << "db_manager is nullptr";
			return;
		}
		DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
		if (db_connector == nullptr) {
			WLOG << "db_connector is nullptr";
			return;
		}
		std::string clause;
		bool first = true;
		for (auto it = group_id_map.begin(); it != group_id_map.end(); it ++) {
			if (first) {
				first = false;
				clause = Util::Uint32ToString(it->first);
			} else {
				clause += ("," + Util::Uint32ToString(it->first));
			}
		}

		std::string sql = "SELECT * FROM ImGroup WHERE id IN (" + clause + ") ORDER BY updated DESC";
		ResultSet *result = db_connector->Query(sql);
		if (result == NULL) {
			WLOG << sql << " failed";
		} else {
			while (result->Next()) {
				uint32_t group_id = result->GetInt("id");
				uint32_t version = result->GetInt("version");
				if (group_id_map[group_id].version() < version) {
					Im::Base::GroupInfo group_info;
					group_info.set_group_id(group_id);
					group_info.set_version(version);
					group_info.set_group_name(result->GetString("name"));
					group_info.set_group_avatar(result->GetString("avatar"));
					Im::Base::GroupType group_type = Im::Base::GroupType(result->GetInt("type"));
					if (Im::Base::GroupType_IsValid(group_type)) {
						group_info.set_group_type(group_type);
						group_info.set_from_user_id(result->GetInt("creator"));
						group_info_list.push_back(group_info);
					} else {
						WLOG << "group type is invalid";
					}
				}
			}
			delete result;
		}
		db_manager->ReleaseDatabaseConnector(db_connector);
		if (!group_info_list.empty()) {
			FillGroupMember(group_info_list);
		} else {
			DLOG << "group_info_list is empty";
		}

	} else {
		DLOG << "group id map is empty";
	}
}

bool GroupModel::SetPush(uint32_t user_id, uint32_t group_id, uint32_t type, uint32_t status) {
	
	bool ret = false;
	if (!IsInGroup(user_id, group_id)) {
		WLOG << "user id => " << user_id << " is not in group_id => " << group_id;
		return ret;  
	}

	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return ret;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_set");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return ret;
	}

	std::string key = "group_set_" + Util::Uint32ToString(group_id);
	std::string field = Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(type);
	int res = cache_connector->Hset(key, field, Util::Uint32ToString(status));
	cache_manager->ReleaseCacheConnector(cache_connector);
	if (res != -1) {
		ret = true;
	} else {	
		WLOG << "hset failed";
	}
	return ret;
}

void GroupModel::GetPush(uint32_t group_id, std::list<uint32_t>& user_list, 
	std::list<Im::Base::ShieldStatus>& push_list) {
	
	if (user_list.empty()) {
		WLOG << "user_list is empty";
		return;
	}

	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_set");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return;
	}

	std::string key = "group_set_" + Util::Uint32ToString(group_id);
	std::map<std::string, std::string> ret_map;
	bool ret = cache_connector->HgetAll(key, ret_map);
	cache_manager->ReleaseCacheConnector(cache_connector);

	if (ret) {
		for (auto it = user_list.begin(); it != user_list.end(); it ++) {
			Im::Base::ShieldStatus status;
			status.set_group_id(group_id);
			status.set_user_id(*it);
			std::string field = Util::Uint32ToString(*it) + "_" + Util::Uint32ToString(IM_GROUP_SETTING_PUSH);
			auto i = ret_map.find(field);
			if (i != ret_map.end()) {
				status.set_shield_status(Util::StringToUint32(i->second));
			} else {
				status.set_shield_status(0);
			}
			push_list.push_back(status);
		}
	} else {
		WLOG << "hgetAll failed, key => " + key;
	}
}

bool GroupModel::ModifyGroupMember(uint32_t user_id, uint32_t group_id, 
	Im::Base::GroupModifyType type, std::set<uint32_t>& user_id_set, std::list<uint32_t>& cur_user_id_list) {
	
	bool ret = false;
	if (HasModifyPermission(user_id, group_id, type)) {
		switch (type) {
			case Im::Base::GROUP_MODIFY_TYPE_ADD:
				ret = AddMember(group_id, user_id_set, cur_user_id_list);
				break;
			case Im::Base::GROUP_MODIFY_TYPE_DEL:
				ret = RemoveMember(group_id, user_id_set, cur_user_id_list);
				RemoveSession(group_id, user_id_set);
				break;
			default:
				WLOG << "modify group member type => " << type;
				break;
		}
		if (ret) {
			IncGroupVersion(group_id);
			for (auto it = user_id_set.begin(); it != user_id_set.end(); it ++) {
				uint32_t user_id = *it;
				UserModel::GetInstance()->ClearUserCounter(user_id, group_id, Im::Base::SESSION_TYPE_GROUP);
			}
		}
	} else {
		WLOG << "not modify permission";
	}
	return ret;
}

void GroupModel::GetGroupUser(uint32_t group_id, std::list<uint32_t>& user_id_list) {
	
	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return;
	}

	std::string key = "group_member_" + Util::Uint32ToString(group_id);
	std::map<std::string, std::string> all_user_map;
	bool ret = cache_connector->HgetAll(key, all_user_map);
	cache_manager->ReleaseCacheConnector(cache_connector);

	if (ret) {
		for (auto it = all_user_map.begin(); it != all_user_map.end(); it ++) {
			uint32_t user_id = Util::StringToUint32(it->first);
			user_id_list.push_back(user_id);
		}
	} else {
		WLOG << "hgetAll failed, key => " << key;
	}
}

bool GroupModel::IsInGroup(uint32_t user_id, uint32_t group_id) {
	
	bool ret = false;
	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return ret;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return ret;
	}

	std::string key = "group_member_" + Util::Uint32ToString(group_id);
	std::string field = Util::Uint32ToString(user_id);
	std::string value = cache_connector->Hget(key, field);
	cache_manager->ReleaseCacheConnector(cache_connector);

	if (!value.empty()) {
		ret = true;
	} else {
		WLOG << "hget failed, key => " << key;
	}

	return ret;
}

void GroupModel::UpdateGroupChat(uint32_t group_id) {
	
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return;
	}

	uint32_t now = (uint32_t)time(NULL);
	std::string sql = "UPDATE ImGroup SET last_chated=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(group_id);
	if (!db_connector->Update(sql)) {
		WLOG << sql << " failed";
	}

	db_manager->ReleaseDatabaseConnector(db_connector);
}

bool GroupModel::IsInvalidateGroupId(uint32_t group_id) {
	
	bool ret = false;
	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return ret;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return ret;
	}

	std::string key = "group_member_" + Util::Uint32ToString(group_id);
	ret = cache_connector->IsExists(key);

	cache_manager->ReleaseCacheConnector(cache_connector);

	return ret;
}

uint32_t GroupModel::GetUserJoinTime(uint32_t group_id, uint32_t user_id) {
	
	uint32_t ret = 0;
	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return ret;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return ret;
	}

	std::string key = "group_member_" + Util::Uint32ToString(group_id);
	std::string field = Util::Uint32ToString(user_id);
	std::string value = cache_connector->Hget(key, field);
	if (!value.empty()) {
		ret = Util::StringToUint32(value);
	}
	cache_manager->ReleaseCacheConnector(cache_connector);

	return ret;
}

bool GroupModel::InsertNewGroup(uint32_t user_id, const std::string& group_name, 
	const std::string& group_avatar, uint32_t group_type, uint32_t member_cnt, uint32_t& group_id) {
	
	bool ret = false;
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}

	std::string insert = "INSERT INTO ImGroup (`name`, `avatar`, `creator`, `type`, `user_cnt`, `status`, `version`, `last_chated`, `created`, `updated`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	PrepareStatement stmt;
	if (stmt.Init(db_connector->GetMysql(), insert)) {
		uint32_t now = (uint32_t) time(NULL);
		int index = 0;
		uint32_t type = group_type;
		stmt.SetParam(index ++, group_name);
		stmt.SetParam(index ++, group_avatar);
		stmt.SetParam(index ++, user_id);
		stmt.SetParam(index ++, type);
		stmt.SetParam(index ++, 0);
		stmt.SetParam(index ++, 0);
		stmt.SetParam(index ++, 0);
		stmt.SetParam(index ++, now);
		stmt.SetParam(index ++, now);
		ret = stmt.Execute();
		if (!ret) {
			WLOG << insert << " failed";
		}
	} else {
		WLOG << "stmt init failed";
	}

	db_manager->ReleaseDatabaseConnector(db_connector);
	return ret;
}

bool GroupModel::InsertNewMember(uint32_t group_id, std::set<uint32_t>& user_set) {
	
	bool ret = false;
	uint32_t user_cnt = user_set.size();
	if (group_id != INVALID_VALUE && user_cnt > 0) {
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == nullptr) {
			WLOG << "db_manager is nullptr";
			return ret;
		}
		DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
		if (db_connector == nullptr) {
			WLOG << "db_connector is nullptr";
			return ret;
		}
		uint32_t now = (uint32_t)time(NULL);
		std::string clause;
		bool first = true;
		for (auto it = user_set.begin(); it != user_set.end(); it ++) {
			if (first) {
				first = false;
				clause = Util::Uint32ToString(*it);
			} else {
				clause += ("," + Util::Uint32ToString(*it));
			}
		}
		std::string sql = "SELECT user_id FROM ImGroupMember WHERE group_id=" + Util::Uint32ToString(group_id) + " AND user_id IN (" + clause + ")";
		ResultSet *result = db_connector->Query(sql);
		std::set<uint32_t> user_id_set;
		if (result == NULL) {
			DLOG << sql << " failed";
		} else {
			while (result->Next()) {
				user_id_set.insert(result->GetInt("user_id"));
			}
			delete result;
		}

		// should user master db.
		if (!user_id_set.empty()) {
			clause.clear();
			first = true;
			for (auto it = user_id_set.begin(); it != user_id_set.end(); it ++) {
				if (first) {
					first = false;
					clause = Util::Uint32ToString(*it);
				} else {
					clause += ("," + Util::Uint32ToString(*it));
				}
			}
			sql = "UPDATE ImGroupMember SET status=0, updated=" + Util::Uint32ToString(now) + " WHERE group_id=" + Util::Uint32ToString(group_id) + " AND user_id IN (" + clause + ")";
			if (!db_connector->Update(sql)) {
				WLOG << sql << " failed";
			}
		}

		std::string insert = "INSERT INTO ImGruopMember (`group_id`, `user_id`, `status`, `created`, `updated`) VALUES (?, ?, ?, ?, ?)";	

		int insert_cnt = 0;
		for (auto it = user_set.begin(); it != user_set.end(); it ++) {
			uint32_t user_id = *it;
			if (user_id_set.find(user_id) == user_id_set.end()) {
				PrepareStatement stmt;
				if (stmt.Init(db_connector->GetMysql(), insert)) {
					int index = 0;
					stmt.SetParam(index ++, group_id);
					stmt.SetParam(index ++, user_id);
					stmt.SetParam(index ++, 0);
					stmt.SetParam(index ++, now);
					stmt.SetParam(index ++, now);
					if (!stmt.Execute()) {
						WLOG << insert << " failed";
					} else {
						insert_cnt ++;
					}
				} else {
					WLOG <<  "stmt init failed";
				}
			}
		}
		if (insert_cnt != 0) {
			sql = "UPDATE ImGroup SET user_cnt=user_cnt+" + Util::Uint32ToString(insert_cnt) + " WHERE id=" + Util::Uint32ToString(group_id);
			if (!db_connector->Update(sql)) {
				WLOG << sql << " failed";
			}
		}
		db_manager->ReleaseDatabaseConnector(db_connector);

		CacheManager* cache_manager = CacheManager::GetInstance();
		if (cache_manager == nullptr) {
			WLOG << "cache_manager is nullptr";
			return ret;
		}
		CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
		if (cache_connector == nullptr) {
			WLOG << "cache_connector is nullptr";
			return ret;
		}
		std::string key = "group_member_" + Util::Uint32ToString(group_id);
		for (auto it = user_set.begin(); it != user_set.end(); it ++) {
			cache_connector->Hset(key, Util::Uint32ToString(*it), Util::Uint32ToString(now));
		}
		cache_manager->ReleaseCacheConnector(cache_connector);

		ret = true;
	} else {
		DLOG << "insert new member failed";
	}

	return ret;
}

std::string GroupModel::GenerateGroupAvatar(uint32_t group_id) {

}

void GroupModel::GetGroupVersion(std::list<uint32_t>& group_id_list, 
	std::list<Im::Base::GroupVersionInfo>& group_list, uint32_t group_type) {
	
	if (!group_id_list.empty()) {
		DatabaseManager* db_manager = DatabaseManager::GetInstance();
		if (db_manager == nullptr) {
			WLOG << "db_manager is nullptr";
			return;
		}
		DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
		if (db_connector == nullptr) {
			WLOG << "db_connector is nullptr";
			return;
		}

		std::string clause;
		bool first = true;
		for (auto it = group_id_list.begin(); it != group_id_list.end(); it ++) {
			if (first) {
				first = false;
				clause = Util::Uint32ToString(*it);
			} else {
				clause += ("," + Util::Uint32ToString(*it));
			}
		}
		std::string sql = "SELECT id, version FROM ImGroup WHERE id IN (" + clause + ")";
		if (group_type != 0) {
			sql += " AND type=" + Util::Uint32ToString(group_type);
		}
		sql += " ORDER BY updated DESC";
		ResultSet *result = db_connector->Query(sql);
		if (result == NULL) {
			DLOG << sql << " failed";
		} else {
			Im::Base::GroupVersionInfo group;
			while (result->Next()) {
				group.set_group_id(result->GetInt("id"));
				group.set_version(result->GetInt("version"));
				group_list.push_back(group);
			}
			delete result;
		}

		db_manager->ReleaseDatabaseConnector(db_connector);
	} else {
		DLOG << "group_id_list is empty";
	}
}

bool GroupModel::HasModifyPermission(uint32_t user_id, uint32_t group_id, Im::Base::GroupModifyType type) {
	
	bool ret = false;
	if (user_id == 0) {
		return true;
	}
	
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}

	std::string sql = "SELECT creator, type FROM ImGroup WHERE id=" + Util::Uint32ToString(group_id);
	ResultSet *result = db_connector->Query(sql);
	if (result == NULL) {
		WLOG << sql << " failed";
	} else {
		while (result->Next()) {
			uint32_t create_id = result->GetInt("creator");
			Im::Base::GroupType group_type = Im::Base::GroupType(result->GetInt("type"));
			if (Im::Base::GroupType_IsValid(group_type)) {
				if (Im::Base::GROUP_TYPE_TMP == group_type && Im::Base::GROUP_MODIFY_TYPE_ADD == type) {
					ret = true;
					break;
				} else {
					if (create_id == user_id) {
						ret = true;
						break;
					}
				}
			}
		}
		delete result;
	}
	db_manager->ReleaseDatabaseConnector(db_connector);
	return ret;
}

bool GroupModel::AddMember(uint32_t group_id, std::set<uint32_t>& user_set, 
	std::list<uint32_t>& cur_user_id_list) {
	RemoveRepeatUser(group_id, user_set);
	bool ret = InsertNewMember(group_id, user_set);
	GetGroupUser(group_id, cur_user_id_list);
	return ret;
}

bool GroupModel::RemoveMember(uint32_t group_id, std::set<uint32_t>& user_set,
 	std::list<uint32_t>& cur_user_id_list) {
	if (user_set.empty()) {
		return true;
	}
	bool ret = false;
	std::string clause;
	bool first = true;
	for (auto it = user_set.begin(); it != user_set.end(); it ++) {
		if (first) {
			first = false;
			clause = Util::Uint32ToString(*it);
		} else {
			clause += ("," + Util::Uint32ToString(*it));
		}
	}
	std::string sql = "UPDATE ImGroupMember SET status=1 WHERE group_id=" + Util::Uint32ToString(group_id) + " AND user_id IN (" + clause + ")";
	
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}
	if (db_connector->Update(sql)) {
		WLOG << sql << " failed";
	}
	db_manager->ReleaseDatabaseConnector(db_connector);

	std::string key = "group_member_" + Util::Uint32ToString(group_id);

	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return ret;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return ret;
	}
	for (auto it = user_set.begin(); it != user_set.end(); it ++) {
		std::string field = Util::Uint32ToString(*it);
		cache_connector->Hdel(key, field);
	}
	cache_manager->ReleaseCacheConnector(cache_connector);

	return true;

}

void GroupModel::RemoveRepeatUser(uint32_t group_id, std::set<uint32_t>& user_set) {
	std::string key = "group_member_" + Util::Uint32ToString(group_id);

	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return;
	}
	for (auto it = user_set.begin(); it != user_set.end(); ) {
		std::string field = Util::Uint32ToString(*it);
		std::string value = cache_connector->Hget(key, field);
		if (value.empty()) {
			it ++;
		} else {
			it = user_set.erase(it);
		}
	}
	cache_manager->ReleaseCacheConnector(cache_connector);
}

void GroupModel::RemoveSession(uint32_t group_id, const std::set<uint32_t>& user_set) {
	for (auto it = user_set.begin(); it != user_set.end(); it ++) {
		uint32_t user_id = *it;
		uint32_t session_id = SessionModel::GetInstance()->GetSessionId(user_id, group_id, Im::Base::SESSION_TYPE_GROUP, false);
		SessionModel::GetInstance()->RemoveSession(session_id);
	}
}

bool GroupModel::IncGroupVersion(uint32_t group_id) {
	
	bool ret = false;
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return ret;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return ret;
	}

	std::string sql = "UPDATE ImGroup SET version=version+1 WHERE id=" + Util::Uint32ToString(group_id);
	if (db_connector->Update(sql)) {
		ret = true;
	}

	db_manager->ReleaseDatabaseConnector(db_connector);

	return ret;
}

void GroupModel::ClearGroupMember(uint32_t group_id) {
	
	DatabaseManager* db_manager = DatabaseManager::GetInstance();
	if (db_manager == nullptr) {
		WLOG << "db_manager is nullptr";
		return;
	}
	DatabaseConnector* db_connector = db_manager->GetDatabaseConnector("database_master");
	if (db_connector == nullptr) {
		WLOG << "db_connector is nullptr";
		return;
	}
	std::string sql = "DELETE FROM ImGroupMember WHERE group_id=" + Util::Uint32ToString(group_id);
	if (db_connector->Update(sql.c_str())) {
		DLOG << "DELETE " << db_connector->GetInsertId();
	} else {
		WLOG << sql << " failed";
		db_manager->ReleaseDatabaseConnector(db_connector);
		return;
	}
	db_manager->ReleaseDatabaseConnector(db_connector);

	CacheManager* cache_manager = CacheManager::GetInstance();
	if (cache_manager == nullptr) {
		WLOG << "cache_manager is nullptr";
		return;
	}
	CacheConnector* cache_connector = cache_manager->GetCacheConnector("group_member");
	if (cache_connector == nullptr) {
		WLOG << "cache_connector is nullptr";
		return;
	}
	std::string key = "group_member_" + Util::Uint32ToString(group_id);
	std::map<std::string, std::string> ret_map;
	if (cache_connector->HgetAll(key, ret_map)) {
		for (auto it = ret_map.begin(); it != ret_map.end(); it ++) {
			cache_connector->Hdel(key, it->first);
		}
	} else {
		WLOG << "hgetAll failed";
	}

	cache_manager->ReleaseCacheConnector(cache_connector);
	
}

void GroupModel::FillGroupMember(std::list<Im::Base::GroupInfo>& group_list) {
	for (auto it = group_list.begin(); it != group_list.end(); it ++) {
		uint32_t group_id = it->group_id();
		std::list<uint32_t> user_id_list;
		GetGroupUser(group_id, user_id_list);
		for (auto user_id = user_id_list.begin(); user_id != user_id_list.end(); user_id ++) {
			it->add_group_member_list(*user_id);
		}
	}
}

} // namespace marz
