#include "SessionModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "Types.h"
#include "Util.h"
#include "MessageModel.h"
#include "GroupMessageModel.h"

namespace marz {

SessionModel *SessionModel::instance_ = NULL;

SessionModel::SessionModel()
{
}

SessionModel::~SessionModel()
{
}

SessionModel *SessionModel::GetInstance() {
	if (instance_ == NULL) {
		instance_ = new SessionModel();
	}
	return instance_;
}

void SessionModel::GetRecentSession(uint32_t user_id, uint32_t last_time, 
	std::list<Im::Base::ContactSessionInfo>& contact_list) {

	std::string sql = "SELECT * FROM ImRecentSession WHERE user_id=" + Util::Uint32ToString(user_id) + " AND status=0 AND updated>=" + Util::Uint32ToString(last_time) + " ORDER BY updated DESC LIMIT 100";  

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {

			while (result->Next()) {
				Im::Base::ContactSessionInfo contact;
				contact.set_session_id(result->GetInt("peer_id"));
				contact.set_session_status(Im::Base::SessionStatusType(result->GetInt("status")));
				Im::Base::SessionType session_type = Im::Base::SessionType(result->GetInt("type"));
				if (Im::Base::SessionType_IsValid(session_type)) {
					contact.set_session_type(session_type);
					contact.set_update_time(result->GetInt("updated"));
					contact_list.push_back(contact);
				} else {
					WLOG << "session type is invalid";
				}
			}

			delete result;
		} else {
			WLOG << sql << " failed";
		}

		if (!contact_list.empty()) {
			FillSessionMsg(user_id, contact_list);
		}
	} else {
		WLOG << "db connector is NULL";
	}
}

uint32_t SessionModel::GetSessionId(uint32_t user_id, uint32_t peer_id, uint32_t type, bool isAll) {
	uint32_t session_id = INVALID_VALUE;
	std::string sql;
	if (isAll) {
		sql = "SELECT id FROM ImRecentSession WHERE user_id=" + Util::Uint32ToString(user_id) + " AND peer_id=" + Util::Uint32ToString(peer_id) + " AND type=" + Util::Uint32ToString(type);  
	} else {
		sql = "SELECT id FROM ImRecentSession WHERE user_id=" + Util::Uint32ToString(user_id) + " AND peer_id=" + Util::Uint32ToString(peer_id) + " AND type=" + Util::Uint32ToString(type) +" AND status=0";  
	}

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {

			while (result->Next()) {
				session_id = result->GetInt("id");
			}
			
			delete result;
		} else {
			WLOG << sql << " failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}

	return session_id;
}

uint32_t SessionModel::AddSession(uint32_t user_id, uint32_t peer_id, uint32_t type)
{
	uint32_t session_id = GetSessionId(user_id, peer_id, type, true);
	uint32_t now = (uint32_t)time(NULL);
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		if (session_id == INVALID_VALUE) {
			std::string sql = "UPDATE ImRecentSession SET statu=0, updated=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(session_id);
			if (!db_connector->Update(sql)) {
				DLOG << sql << " failed";
				session_id = INVALID_VALUE;
			}
		} else {
			std::string insert = "INSERT INTO ImRecentSession (`user_id`, `peer_id`, `type`, `status`, `created`, `updated`) VALUES (?, ?, ?, ?, ?, ?)";
			PrepareStatement stmt;
			if (stmt.Init(db_connector->GetMysql(), insert)) {
				int index = 0;

				stmt.SetParam(index ++, user_id);
				stmt.SetParam(index ++, peer_id);
				stmt.SetParam(index ++, type);
				stmt.SetParam(index ++, 0);
				stmt.SetParam(index ++, now);
				stmt.SetParam(index ++, now);
				if (stmt.Execute()) {
					session_id = stmt.GetInsertId();
				} else {
					WLOG << "stmt.execute is failed, insert => " << insert;
				}
			} else {
				WLOG << "stmt.init is failed, insert => " << insert;
			}
		}
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "db connector is NULL";
	}

	return session_id;
}

bool SessionModel::UpdateSession(uint32_t session_id, uint32_t update_time)
{
	uint32_t ret = false;
	std::string sql = "UPDATE ImRecentSession SET `updated`=" + Util::Uint32ToString(update_time) + " WHERE id=" + Util::Uint32ToString(session_id);
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ret = db_connector->Update(sql);
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "db connector is NULL";
	}

	return ret;
}

bool SessionModel::RemoveSession(uint32_t session_id)
{
	uint32_t ret = false;
	uint32_t now = (uint32_t)time(NULL);
	std::string sql = "UPDATE ImRecentSession SET `status`=1, `updated`=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(session_id);
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ret = db_connector->Update(sql);
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "db connector is NULL";
	}

	return ret;
}

void SessionModel::FillSessionMsg(uint32_t user_id, std::list<Im::Base::ContactSessionInfo>& contact_list) {
	
	for (auto it = contact_list.begin(); it != contact_list.end(); ) {
		uint32_t msg_id = 0;
		std::string msg_data;
		Im::Base::MsgType msg_type;
		uint32_t from_id = 0;
		
		if (it->session_type() == Im::Base::SESSION_TYPE_SINGLE) {
			from_id = it->session_id();
			MessageModel::GetInstance()->GetLastMsg(it->session_id(), user_id, msg_id, msg_data, msg_type, from_id);
		} else {
			GroupMessageModel::GetInstance()->GetLastMsg(it->session_id(), msg_id, msg_data, msg_type, from_id);
		}

		if (Im::Base::MsgType_IsValid(msg_type)) {
			it->set_latest_msg_id(msg_id);
			it->set_latest_msg_data(msg_data);
			it->set_latest_msg_type(msg_type);
			it->set_latest_msg_from_user_id(from_id);
		} else {
			it = contact_list.erase(it);
		}
	}
}

} // namespace marz
