#include "UserModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "Util.h"

namespace marz {

UserModel *UserModel::instance_ = NULL;

UserModel::UserModel()
{
}

UserModel::~UserModel()
{
}

UserModel *UserModel::GetInstance() {
	
	if (instance_ == NULL) {
		instance_ = new UserModel();
	}

	return instance_;
}

void UserModel::GetChangedId(uint32_t& last_time, std::list<uint32_t>& id_list) {
	
	std::string sql;
	if (last_time == 0) {
		sql = "SELECT id, updated FROM ImUser WHERE status !=3";
	} else {
		sql = "SELECT id, updated FROM ImUser WHERE updated >=" + Util::Uint32ToString(last_time);
	}

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {
			while (result->Next()) {
				uint32_t id = result->GetInt("id");
				uint32_t updated = result->GetInt("updated");
				if (last_time < updated) {
					last_time = updated;
				}
				id_list.push_back(id);
			}

			delete result;
		} else {
			WLOG << sql << " Failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}
}

void UserModel::GetUserList(std::list<uint32_t> user_id_list, std::list<Im::Base::UserInfo>& user_info_list) {

	if (user_id_list.empty()) {
		WLOG << "user_id_list is empty";
		return;
	}

	std::string clause;
	bool first = true;
	for (auto it = user_id_list.begin(); it != user_id_list.end(); it ++) {
		if (first) {
			first = false;
			clause = Util::Uint32ToString(*it);
		} else {
			clause += ("," + Util::Uint32ToString(*it));
		}
	}
	std::string	sql = "SELECT * FROM ImUser WHERE id IN (" + clause + ")";
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {
			while (result->Next()) {
				Im::Base::UserInfo user;
				user.set_user_id(result->GetInt("id"));
				user.set_sex(result->GetInt("sex"));
				user.set_nick(result->GetString("nick"));
				user.set_avatar(result->GetString("avatar"));
				user.set_phone(result->GetString("phone"));
				user.set_email(result->GetString("email"));
				user.set_name(result->GetString("name"));
				user.set_domain(result->GetString("domain"));
				user.set_status(result->GetInt("status"));

				user_info_list.push_back(user);
			}

			delete result;
		} else {
			WLOG << sql << " failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}
}

bool UserModel::GetUser(uint32_t user_id, user_info_t& user) {

	bool ret = false;
	std::string	sql = "SELECT * FROM ImUser WHERE id=" + Util::Uint32ToString(user_id);
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {
			while (result->Next()) {
				user.user_id = result->GetInt("id");
				user.sex = result->GetInt("sex");
				user.nick = result->GetString("nick");
				user.avatar = result->GetString("avatar");
				user.phone = result->GetString("phone");
				user.email = result->GetString("email");
				user.name = result->GetString("name");
				user.domain = result->GetString("domain");
				user.status = result->GetInt("status");

				ret = true;
			}

			delete result;
		} else {
			WLOG << sql << " failed";
		}
	} else {
		WLOG << "db connector is NULL";
		return ret;
	}

	return ret;
}

bool UserModel::UpdateUser(user_info_t& user) {

	bool ret = false;
	uint32_t now = (uint32_t) time(NULL);
	std::string	sql = "UPDATE ImUser SET `sex`=" + Util::Uint32ToString(user.sex) + ", `name`=" + user.name + ", `domain`=" + user.domain + ", `nick`=" + user.nick + ", `phone`=" + user.phone + ", `email`=" + user.email + ", `avatar`=" + user.avatar + ", `status`=" + Util::Uint32ToString(user.status) + ", `updated`=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(user.user_id);	  
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {
		ret = db_connector->Update(sql);
		DatabaseManager::ReleaseConnector(db_connector);
		if (!ret) {
			WLOG << sql << " is failed";
		}
 	} else {
		WLOG << "db connector is NULL";
		return ret;
	}
}

bool UserModel::InsertUser(user_info_t& user) {

	bool ret = false;
	uint32_t now = (uint32_t) time(NULL);
	std::string	insert = "INSERT INTO ImUser (`id`, `sex`, `name`, `domain`, `nick`, `phone`, `email`, `avatar`, `status`, `created`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";  
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {
		PrepareStatement stmt;
		if (stmt.Init(db_connector->GetMysql(), insert)) {
			int index = 0;
			uint32_t sex = user.sex;
			uint32_t status = user.status;
			stmt.SetParam(index ++, user.user_id);
			stmt.SetParam(index ++, sex);
			stmt.SetParam(index ++, user.name);
			stmt.SetParam(index ++, user.domain);
			stmt.SetParam(index ++, user.nick);
			stmt.SetParam(index ++, user.phone);
			stmt.SetParam(index ++, user.email);
			stmt.SetParam(index ++, user.avatar);
			stmt.SetParam(index ++, status);
			stmt.SetParam(index ++, now);

			ret = stmt.Execute();
			if (!ret) {
				WLOG << "stmt.execute is failed";
			}
		} else {
			WLOG << "stmt.init is failed sql => " << insert;
		}
		DatabaseManager::ReleaseConnector(db_connector);
 	} else {
		WLOG << "db connector is NULL";
		return ret;
	}
}

void UserModel::ClearUserCounter(uint32_t user_id, uint32_t peer_id, Im::Base::SessionType session_type) {

	if (Im::Base::SessionType_IsValid(session_type)) {
		CacheConnector *cache_connector = CacheManager::GetConnector("unread");
		if (cache_connector != NULL) {
			if (session_type == Im::Base::SESSION_TYPE_SINGLE) {
				std::string key = "unread_" + Util::Uint32ToString(user_id);
				if (!cache_connector->Hdel(key, Util::Uint32ToString(peer_id))) {
					WLOG << "hdel key => " << key << ", peer_id => " << peer_id << " is failed";
				}
			} else if (session_type == Im::Base::SESSION_TYPE_GROUP) {
				std::string key = GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(peer_id);
				std::map<std::string, std::string> ret_map;
				if (cache_connector->HgetAll(key, ret_map)) {
					key = GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(peer_id);
					std::string reply = cache_connector->Hmset(key, ret_map);
					if (reply.empty()) {
						WLOG << "hmset is failed";
					}
				} else {	
					WLOG << "hgetAll is failed, key => " << key;
				}
			}
			
			CacheManager::ReleaseConnector(cache_connector);
		} else {
			WLOG << "cache connector is NULL";
		}
	} else {
		WLOG << "session type is invalid";
	}
}

} // namespace marz
