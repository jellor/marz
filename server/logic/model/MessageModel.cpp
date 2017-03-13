#include "MessageModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "Types.h"
#include "Util.h"
#include "AudioModel.h"
#include "RelationModel.h"

namespace marz {

MessageModel* MessageModel::instance_ = NULL;

MessageModel::MessageModel()
{
}

MessageModel::~MessageModel()
{
}

MessageModel* MessageModel::GetInstance() {
	if (instance_ == NULL) {
		instance_ = new MessageModel();
	}

	return instance_;
}

bool MessageModel::SendMessage(uint32_t relate_id, uint32_t from_id, 
	uint32_t to_id, Im::Base::MsgType msg_type, uint32_t create_time, uint32_t msg_id, std::string& msg_content) {
	
	bool ret = false;
	if (from_id == 0 || to_id == 0) {
		WLOG << "from_id or to_id is invalid";
		return ret;
	}

	//uint32_t now = (uint32_t)time(NULL);
	std::string table_name = "ImMessage_" + Util::Uint32ToString(relate_id % 8);
	std::string insert = "INSERT INTO " + table_name + " (`relate_id`, `from_id`, `to_id`, `msg_id`, `content`, `type`, `status`, `created`, `updated`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
			
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		PrepareStatement stmt;
		if (stmt.Init(db_connector->GetMysql(), insert)) {
			int index = 0;
			uint32_t type = msg_type;
			stmt.SetParam(index ++, relate_id);
			stmt.SetParam(index ++, to_id);
			stmt.SetParam(index ++, msg_id);
			stmt.SetParam(index ++, msg_content);
			stmt.SetParam(index ++, type);
			stmt.SetParam(index ++, 0);
			stmt.SetParam(index ++, create_time);
			stmt.SetParam(index ++, create_time);
			if (ret = stmt.Execute()) {
				IncMsgCnt(from_id, to_id);
			} else {
				WLOG << "stmt.execute is failed, insert => " << insert;
			}
		} else {
			WLOG << "stmt.init is failed, insert => " << insert;
		}
		
	} else {
		WLOG << "db connector is NULL";
	}
	DatabaseManager::ReleaseConnector(db_connector);

	return ret;
}

bool MessageModel::SendAudioMessage(uint32_t relate_id, uint32_t from_id, 
	uint32_t to_id, Im::Base::MsgType msg_type, uint32_t create_time, 
	uint32_t msg_id, const char *msg_content, uint32_t msg_len) {
	
	bool ret = false;
	if (msg_len <= 4) {
		WLOG << "msg_len is invalid";
		return ret;
	}

	int audio_id = AudioModel::GetInstance()->SaveAudio(from_id, to_id, create_time, msg_content, msg_len);
	if (audio_id != -1) {
		std::string msg_content = Util::Uint32ToString(audio_id);
		ret = SendMessage(relate_id, from_id, to_id, msg_type, create_time, msg_id, msg_content);
	} else {
		ret = false;
	}

	return ret;
}

void MessageModel::GetMessage(uint32_t user_id, uint32_t peer_id,
 	uint32_t msg_id, uint32_t msg_cnt, std::list<Im::Base::MsgInfo>& msg_info_list) {
	uint32_t relate_id = RelationModel::GetInstance()->GetRelationId(user_id, peer_id, false);
	if (relate_id != INVALID_VALUE) {
		std::string table_name = "ImMessage_" + Util::Uint32ToString(relate_id % 8);
		std::string sql;
		if (msg_id == 0) {
			sql = "SELECT * FROM " + table_name + " FORCE INDEX (idx_relate_status_created) WHERE relate_id=" + Util::Uint32ToString(relate_id) + " AND status=0 ORDER BY created DESC, id DESC LIMIT " + Util::Uint32ToString(msg_cnt);   
		} else {
			sql = "SELECT * FROM " + table_name + " FORCE INDEX (idx_relate_status_created) WHERE relate_id=" + Util::Uint32ToString(relate_id) + " AND msg_id<=" + Util::Uint32ToString(msg_id) + " AND status=0 ORDER BY created DESC, id DESC LIMIT " + Util::Uint32ToString(msg_cnt);   
		}

		DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
		if (db_connector != NULL) {	
			ResultSet *result = db_connector->Query(sql);
			DatabaseManager::ReleaseConnector(db_connector);

			if (result != NULL) {
				while (result->Next()) {
					Im::Base::MsgInfo msg_info;
					msg_info.set_msg_id(result->GetInt("msg_id"));
					msg_info.set_from_session_id(result->GetInt("from_id"));
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
		WLOG << "relate_id is invalid";
	}
}

bool MessageModel::ClearMsgCnt(uint32_t user_id, uint32_t peer_id) {

}

//简单全局msg_id生成
uint32_t MessageModel::GetMsgId(uint32_t relate_id) {

	uint32_t msg_id;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = "msg_id_" + Util::Uint32ToString(relate_id);
		msg_id = cache_connector->IncrBy(key, 1);
		
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}

	return msg_id;
}

void MessageModel::GetUnreadMsgCnt(uint32_t user_id, uint32_t& total_cnt, 
	std::list<Im::Base::UnreadInfo>& unread_info_list) {
	
	std::string key = "unread_" + Util::Uint32ToString(user_id);
	std::map<std::string, std::string> ret_map;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		bool ret = cache_connector->HgetAll(key, ret_map);	
		CacheManager::ReleaseConnector(cache_connector);

		if (ret) {
			Im::Base::UnreadInfo unread_info;
			for (auto it = ret_map.begin(); it != ret_map.end(); it ++) {
				unread_info.set_session_id(Util::StringToUint32(it->first));
				unread_info.set_unread_count(Util::StringToUint32(it->second));
				unread_info.set_session_type(Im::Base::SESSION_TYPE_SINGLE);
				uint32_t msg_id = 0;
				std::string msg_content;
				Im::Base::MsgType msg_type = Im::Base::MsgType(INVALID_VALUE);
				GetLastMsg(unread_info.session_id(), user_id, msg_id, msg_content, msg_type);
				if (Im::Base::MsgType_IsValid(msg_type)) {
					unread_info.set_latest_msg_id(msg_id);
					unread_info.set_latest_msg_data(msg_content);
					unread_info.set_latest_msg_type(msg_type);
					unread_info.set_latest_msg_from_user_id(unread_info.session_id());

					unread_info_list.push_back(unread_info);
					total_cnt += unread_info.unread_count();
				} else {
					WLOG << "msg_type is invalid";
				}
			}
		} else {
			WLOG << "hgetAll is failed, key => " << key;
		}
	} else {
		WLOG << "cache connector is NULL";
	}
}

void MessageModel::GetLastMsg(uint32_t from_id, uint32_t to_id, 
	uint32_t& msg_id, std::string& msg_content, Im::Base::MsgType& msg_type, uint32_t status) {
	
	uint32_t relate_id = RelationModel::GetInstance()->GetRelationId(from_id, to_id, false);
	if (relate_id != INVALID_VALUE) {
		std::string table_name = "ImMessage_" + Util::Uint32ToString(relate_id % 8);
		std::string sql = "SELECT msg_id, content, type FROM " + table_name + " FORCE INDEX (idx_relate_status_created) WHERE relate_id=" + Util::Uint32ToString(relate_id) + " AND status=" + Util::Uint32ToString(status) + " ORDER BY created DESC, id DESC LIMIT 1";   

		DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
		if (db_connector != NULL) {	
			ResultSet *result = db_connector->Query(sql);
			DatabaseManager::ReleaseConnector(db_connector);

			if (result != NULL) {
				while (result->Next()) {
					msg_id = result->GetInt("msg_id");
					Im::Base::MsgType msg_type = Im::Base::MsgType(result->GetInt("type"));

					if (msg_type == Im::Base::MSG_TYPE_SINGLE_AUDIO) {
						// msg_content =  audio.
					} else {
						msg_content = result->GetString("content");
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
		WLOG << "relate_id is invalid";
	}
}

void MessageModel::GetUnreadCntAll(uint32_t user_id, uint32_t& total_cnt) {
	
	std::string key = "unread_" + Util::Uint32ToString(user_id);
	std::map<std::string, std::string> ret_map;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		bool ret = cache_connector->HgetAll(key, ret_map);	
		CacheManager::ReleaseConnector(cache_connector);

		if (ret) {
			for (auto it = ret_map.begin(); it != ret_map.end(); it ++) {
				total_cnt += Util::StringToUint32(it->second);
			}
		} else {
			WLOG << "hgetAll is failed, key => " << key;
		}
	} else {
		WLOG << "cache connector is NULL";
	}
}

void MessageModel::GetMsgByMsgId(uint32_t user_id, uint32_t peer_id, 
	const std::list<uint32_t>& msg_id_list, std::list<Im::Base::MsgInfo>& msg_info_list) {
	if (msg_id_list.empty()) {
		WLOG << "msg_id_list is empty";
		return;
	}

	uint32_t relate_id = RelationModel::GetInstance()->GetRelationId(user_id, peer_id, false);
	if (relate_id == INVALID_VALUE) {
		WLOG << "relate_id is invalid";
		return;
	}

	std::string table_name = "ImMessage_" + Util::Uint32ToString(relate_id % 8);
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
	std::string sql = "SELECT * FROM " + table_name + " WHERE relate_id=" + Util::Uint32ToString(relate_id) + " AND status=0 AND msg_id IN (" + clause + ") ORDER BY created DESC, id DESC LIMIT 100";

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);
		if (result != NULL) {
			while (result->Next()) {
				Im::Base::MsgInfo msg_info;
				msg_info.set_msg_id(result->GetInt("msg_id"));
				msg_info.set_from_session_id(result->GetInt("from_id"));
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

	if (!msg_info_list.empty()) {
		AudioModel::GetInstance()->ReadAudios(msg_info_list);
	}
}

bool MessageModel::ResetMsgId(uint32_t relate_id) {
	bool ret = false;
	uint32_t msg_id;
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = "msg_id_" + Util::Uint32ToString(relate_id);
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

void MessageModel::IncMsgCnt(uint32_t from_id, uint32_t to_id) {
	CacheConnector *cache_connector = CacheManager::GetConnector("unread");
	if (cache_connector != NULL) {
		std::string key = "unread_" + Util::Uint32ToString(to_id);
		cache_connector->HincrBy(key, Util::Uint32ToString(from_id), 1);
		
		CacheManager::ReleaseConnector(cache_connector);
	} else {
		WLOG << "cache connector is NULL";
	}
}

} // namespace marz
