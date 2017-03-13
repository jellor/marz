#include "GroupMessageModel.h"
#include "AudioModel.h"
#include "GroupModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "Types.h"
#include "Util.h"

namespace marz {

GroupMessageModel* GroupMessageModel::instance_ = NULL;

GroupMessageModel::GroupMessageModel()
{
}

GroupMessageModel::~GroupMessageModel(){
}

GroupMessageModel *GroupMessageModel::GetInstance() {
	
	if (instance_ == NULL) {
		instance_ = new GroupMessageModel();
	}

	return instance_;
}

bool GroupMessageModel::SendMessage(uint32_t from_id, uint32_t group_id, 
	Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, const std::string& msg_content) {
	
	bool ret = false;
	if (GroupModel::GetInstance()->IsInGroup(from_id, group_id)) {
		DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
		if (db_connector != NULL) {
			std::string table_name = "ImGroupMessage_" + Util::Uint32ToString(group_id % 8);
			std::string insert = "INSERT INTO " + table_name + " (`group_id`, `user_id`, `msg_id`, `content`, `type`, `status`, `created`, `updated`) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
			PrepareStatement stmt;
			if (stmt.Init(db_connector->GetMysql(), insert)) {
				int index = 0;
				uint32_t type = msg_type;
				stmt.SetParam(index ++, group_id);
				stmt.SetParam(index ++, from_id);
				stmt.SetParam(index ++, msg_id);
				stmt.SetParam(index ++, msg_content);
				stmt.SetParam(index ++, type);
				stmt.SetParam(index ++, 0);
				stmt.SetParam(index ++, create_time);
				stmt.SetParam(index ++, create_time);
				ret = stmt.Execute();
				if (ret) {
					GroupModel::GetInstance()->UpdateGroupChat(group_id);
					IncMsgCnt(from_id, group_id);
					ClearMsgCnt(from_id, group_id);
				} else {
					WLOG << insert << " failed";
				}
			} else {
				WLOG << "stmt.init failed";
			}
		} else {
			WLOG << "db connector is NULL";
			return ret;
		}
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "isInGroup is false";
	}
}

bool GroupMessageModel::SendAudioMessage(uint32_t from_id, uint32_t group_id, 
	Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, const char *msg_content, uint32_t msg_len) {
	
	bool ret = false;
	if (msg_len <= 4) {
		return ret;
	}

	if (GroupModel::GetInstance()->IsInGroup(from_id, group_id)) {
		AudioModel *audio_model = AudioModel::GetInstance();
		int audio_id = audio_model->SaveAudio(from_id, group_id, create_time, msg_content, msg_len);
		if (audio_id != -1) {
			ret = SendMessage(from_id, group_id, msg_type, create_time, msg_id, Util::Uint32ToString(audio_id));
		} else {
			WLOG << "audio_model.saveAudio is failed";
		}
	} else {
		WLOG << "isInGroup is false";
	}

	return ret;
}

void GroupMessageModel::GetMessage(uint32_t user_id, uint32_t group_id, 
	uint32_t msg_id, uint32_t msg_cnt, std::list<Im::Base::MsgInfo>& msg_info_list) {
	
	uint32_t update_time = GroupModel::GetInstance()->GetUserJoinTime(group_id, user_id);
	std::string table_name = "ImGroupMessage_" + Util::Uint32ToString(group_id % 8);
	std::string sql;
	if (msg_id == 0) {
		sql = "SELECT * FROM " + table_name + " WHERE group_id=" + Util::Uint32ToString(group_id) + " AND status=0 AND created>=" + Util::Uint32ToString(update_time)+ " ORDER BY created DESC, id DESC LIMIT " + Util::Uint32ToString(msg_cnt);
	} else {
		sql = "SELECT * FROM " + table_name + " WHERE group_id=" + Util::Uint32ToString(group_id) + " AND status=0 AND created>=" + Util::Uint32ToString(update_time) + " AND msg_id<=" + Util::Uint32ToString(msg_id) + " ORDER BY created DESC, id DESC LIMIT " + Util::Uint32ToString(msg_cnt);
	}

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		// should release before used result ?
		DatabaseManager::ReleaseConnector(db_connector);
		if (result != NULL) {
			// std::map<Im::Base::MsgInfo>
			while (result->Next()) {
				Im::Base::MsgInfo msg_info;
				msg_info.set_msg_id(result->GetInt("msg_id"));
				msg_info.set_from_session_id(result->GetInt("user_id"));
				msg_info.set_create_time(result->GetInt("created"));
				Im::Base::MsgType msg_type = Im::Base::MsgType(result->GetInt("type"));
				if (Im::Base::MsgType_IsValid(msg_type)) {
					msg_info.set_msg_type(msg_type);
					msg_info.set_msg_data(result->GetString("content"));
					msg_info_list.push_back(msg_info);
				} else {
					WLOG << "msg type is wrong => " << msg_type;
				}
			}

			delete result;
		} else {
			WLOG << sql << " failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}
}

bool GroupMessageModel::ClearMsgCnt(uint32_t user_id, uint32_t group_id) {
	
	bool ret = false;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(group_id);
		std::map<std::string, std::string> ret_map;
		if (cache_connector->HgetAll(key, ret_map)) {
			key = GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(group_id);
			std::string reply = cache_connector->Hmset(key, ret_map);
			if (reply.empty()) {
				WLOG << "hmset failed, reply => " + reply;
			} else {
				ret = true;
			}
		} else {
			WLOG << "hgetAll is failed";
		}
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}

	return ret;
}

uint32_t GroupMessageModel::GetMsgId(uint32_t group_id) {

	uint32_t msg_id = 0;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = "group_msg_id_" + Util::Uint32ToString(group_id);
		msg_id = cache_connector->IncrBy(key, 1);
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}

	return msg_id;
}

// count should let count = 0 in the for block ?
void GroupMessageModel::GetUnreadMsgCnt(uint32_t user_id, uint32_t& total_cnt, 
	std::list<Im::Base::UnreadInfo>& unread_cnt_list) {
	
	uint32_t count = 0;
	std::list<uint32_t> group_id_list;
	GroupModel::GetInstance()->GetUserGroupIdList(user_id, group_id_list, 0);


	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		for (auto it = group_id_list.begin(); it != group_id_list.end(); it ++) {
			uint32_t group_id = *it;
			std::string key = GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(group_id);
			std::string group_cnt = cache_connector->Hget(key, GROUP_COUNTER_FIELD);
			if (group_cnt.empty()) {
				DLOG << "group cnt is empty";
				continue;
			}
			uint32_t group_cnt_int = Util::StringToUint32(group_cnt);
			key = GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(group_id);
			std::string value = cache_connector->Hget(key, GROUP_COUNTER_FIELD);
			uint32_t user_cnt = value.empty() ? 0 : Util::StringToUint32(value);
			if (group_cnt_int >= user_cnt) {
				count = group_cnt_int - user_cnt;
			}
			if (count > 0) {
				Im::Base::UnreadInfo unread_info;
				unread_info.set_session_id(group_id);
				unread_info.set_session_type(Im::Base::SESSION_TYPE_GROUP);
				unread_info.set_unread_count(count);
				total_cnt += count;
				std::string msg_data;
				uint32_t msg_id;
				Im::Base::MsgType msg_type;
				uint32_t from_id;
				GetLastMsg(group_id, msg_id, msg_data, msg_type, from_id);
				if (Im::Base::MsgType_IsValid(msg_type)) {
					unread_info.set_latest_msg_id(msg_id);
					unread_info.set_latest_msg_data(msg_data);
					unread_info.set_latest_msg_type(msg_type);
					unread_info.set_latest_msg_from_user_id(from_id);
					unread_cnt_list.push_back(unread_info);
				} else {
					WLOG << "msg_type is invalid";
				}
			}
		}
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}
}

void GroupMessageModel::GetLastMsg(uint32_t group_id, uint32_t& msg_id, 
	std::string& msg_data, Im::Base::MsgType& msg_type, uint32_t& from_id) {
	
	std::string table_name = "ImGroupMessage_" + Util::Uint32ToString(group_id % 8);
	std::string sql = "SELECT msg_id, type, user_id, content FROM " + table_name + " WHERE group_id=" + Util::Uint32ToString(group_id) + " AND status=0 ORDER BY created DESC, id DESC LIMIT 1";

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);
		if (result != NULL) {
			while (result->Next()) {
				msg_id = result->GetInt("msg_id");
				msg_type = Im::Base::MsgType(result->GetInt("type"));
				from_id = result->GetInt("user_id");
				if (msg_type == Im::Base::MSG_TYPE_GROUP_AUDIO) {
					// msg_data = result->getString("content");
				} else {
					msg_data = result->GetString("content");
				}
			}

			delete result;
		} else {
			WLOG << sql << " failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}
}

// count should let count = 0 in the for block ?
void GroupMessageModel::GetUnreadCntAll(uint32_t user_id, uint32_t& total_cnt) {
	
	uint32_t count = 0;
	std::list<uint32_t> group_id_list;
	GroupModel::GetInstance()->GetUserGroupIdList(user_id, group_id_list, 0);

	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		for (auto it = group_id_list.begin(); it != group_id_list.end(); it ++) {
			uint32_t group_id = *it;
			std::string key = GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(group_id);
			std::string group_cnt = cache_connector->Hget(key, GROUP_COUNTER_FIELD);
			if (group_cnt.empty()) {
				DLOG << "group cnt is empty";
				continue;
			}
			uint32_t group_cnt_int = Util::StringToUint32(group_cnt);
			key = GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(group_id);
			std::string value = cache_connector->Hget(key, GROUP_COUNTER_FIELD);

			uint32_t user_cnt = value.empty() ? 0 : Util::StringToUint32(value);

			if (group_cnt_int >= user_cnt) {
				count = group_cnt_int - user_cnt;
			}
			if (count > 0) {
				total_cnt += count;
			}
		}
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}
}

void GroupMessageModel::GetMsgByMsgId(uint32_t user_id, uint32_t group_id, 
	const std::list<uint32_t>& msg_id_list, std::list<Im::Base::MsgInfo>& msg_info_list) {
	
	if (!msg_id_list.empty()) {
		if (GroupModel::GetInstance()->IsInGroup(user_id, group_id)) {

			std::string table_name = "ImGroupMessage_" + Util::Uint32ToString(group_id % 8);
			uint32_t update_time = GroupModel::GetInstance()->GetUserJoinTime(group_id, user_id);
			std::string clause;
			bool first = true;
			for (auto it = msg_id_list.begin(); it != msg_id_list.end(); it ++) {
				if (first) {
					first = false;
					clause = Util::Uint32ToString(*it);
				} else {
					clause += ("," + Util::Uint32ToString(*it));
				}
			}
			std::string sql = "SELECT * FROM " + table_name + " WHERE group_id=" + Util::Uint32ToString(group_id) + " AND msg_id IN (" + clause + ") AND status=0 AND created>=" + Util::Uint32ToString(update_time) + "ORDER BY created DESC, id DESC LIMIT 100";

			DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
			if (db_connector != NULL) {	
				ResultSet *result = db_connector->Query(sql);
				DatabaseManager::ReleaseConnector(db_connector);
				if (result != NULL) {
					while (result->Next()) {
						Im::Base::MsgInfo msg_info;
						msg_info.set_msg_id(result->GetInt("msg_id"));
						msg_info.set_from_session_id(result->GetInt("user_id"));
						msg_info.set_create_time(result->GetInt("created"));
						Im::Base::MsgType msg_type = Im::Base::MsgType(result->GetInt("type"));
						if (Im::Base::MsgType_IsValid(msg_type)) {
							msg_info.set_msg_type(msg_type);
							msg_info.set_msg_data(result->GetString("content"));
							msg_info_list.push_back(msg_info);
						} else {
							WLOG << "msg type is wrong => " << msg_type;
						}
					}

					delete result;
				} else {
					WLOG << sql << " failed";
				}
			} else {
				WLOG << "db connector is NULL";
			}
		} else {
			WLOG << "user_id " << user_id << " is not in group_id " << group_id; 
		}
	} else {
		WLOG << "msg_id_list is empty";
	}
}

bool GroupMessageModel::ResetMsgId(uint32_t group_id) {
	
	bool ret = false;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = "group_msg_id_" + Util::Uint32ToString(group_id);
		std::string value = "0";
		std::string reply = cache_connector->Set(key, value);
		if (reply == value) {
			ret = true;
		}
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}

	return ret;
}

bool GroupMessageModel::IncMsgCnt(uint32_t user_id, uint32_t group_id) {
	
	bool ret = false;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = GROUP_TOTAL_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(group_id);
		cache_connector->HincrBy(key, GROUP_COUNTER_FIELD, 1);
		std::map<std::string, std::string> ret_map;
		if (cache_connector->HgetAll(key, ret_map)) {
			key = GROUP_USER_MSG_COUNTER_REDIS_KEY_PREFIX + Util::Uint32ToString(user_id) + "_" + Util::Uint32ToString(group_id);
			std::string reply = cache_connector->Hmset(key, ret_map);
			if (reply.empty()) {
				WLOG << "hmset failed, reply => " + reply;
			} else {
				ret = true;
			}
		} else {
			WLOG << "hgetAll is failed";
		}
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}

	return ret;
}

} // namespace marz
