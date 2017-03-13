#include "Im.Base.pb.h"
#include "RelationModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "PrepareStatement.h"
#include "Logger.h"
#include "Types.h"
#include "Util.h"
#include "MessageModel.h"

namespace marz {

RelationModel* RelationModel::instance_ = NULL;

RelationModel::RelationModel()
{
}

RelationModel::~RelationModel()
{
}

RelationModel* RelationModel::GetInstance() {

	if (instance_ == NULL) {
		instance_ = new RelationModel();
	}

	return instance_;
}

uint32_t RelationModel::GetRelationId(uint32_t a_user_id, uint32_t b_user_id, bool add) {
	
	uint32_t relation_id = INVALID_VALUE;
	if (a_user_id == 0 || b_user_id == 0) {
		WLOG << "a_user_id or b_user_id is wrong";
		return relation_id;
	}
	uint32_t small_id, big_id;
	if (a_user_id > b_user_id) {
		small_id = b_user_id;
		big_id = a_user_id;
	} else {
		small_id = a_user_id;
		big_id = b_user_id;
	}
	std::string sql = "SELECT id FROM ImRelation WHERE small_id=" + Util::Uint32ToString(small_id) + "AND big_id=" + Util::Uint32ToString(big_id) + " AND status=0";  

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {
			while (result->Next()) {
				relation_id = result->GetInt("id");
			}
			delete result;
		} else {
			WLOG << sql << " failed";
		}
		if (relation_id == INVALID_VALUE && add) {
			relation_id = AddRelation(small_id, big_id);
		}

	} else {
		WLOG << "db connector is NULL";
	}

	return relation_id;
}

bool RelationModel::UpdateRelation(uint32_t relation_id, uint32_t update_time) {

	uint32_t ret = false;
	std::string sql = "UPDATE ImRelation SET `updated`=" + Util::Uint32ToString(update_time) + " WHERE id=" + Util::Uint32ToString(relation_id);
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ret = db_connector->Update(sql);
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "db connector is NULL";
	}

	return ret;
}

bool RelationModel::RemoveRelation(uint32_t relation_id) {

	uint32_t ret = false;
	uint32_t now = (uint32_t)time(NULL);
	std::string sql = "UPDATE ImRelation SET `status`=1, `updated`=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(relation_id);
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ret = db_connector->Update(sql);
		DatabaseManager::ReleaseConnector(db_connector);
	} else {
		WLOG << "db connector is NULL";
	}

	return ret;
}

uint32_t RelationModel::AddRelation(uint32_t small_id, uint32_t big_id) {

	uint32_t relation_id = INVALID_VALUE;
	uint32_t now = (uint32_t)time(NULL);
	std::string sql = "SELECT id FROM ImRelation WHERE small_id=" + Util::Uint32ToString(small_id) + "AND big_id=" + Util::Uint32ToString(big_id) + " AND status=0";  

	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {	
		ResultSet *result = db_connector->Query(sql);

		if (result != NULL && result->Next()) {
			relation_id = result->GetInt("id");
			sql = "UPDATE ImRelation SET status=0, updated=" + Util::Uint32ToString(now) + " WHERE id=" + Util::Uint32ToString(relation_id);
			if (!db_connector->Update(sql)) {
				relation_id = INVALID_VALUE;
			}
			delete result;
		} else {
			std::string insert = "INSERT INTO ImRelation (`small_id`, `big_id`, `status`, `created`, `updated`) VALUES (?, ?, ?, ?, ?)";  
			PrepareStatement stmt;
			if (stmt.Init(db_connector->GetMysql(), insert)) {
				int index = 0;
				stmt.SetParam(index ++, small_id);
				stmt.SetParam(index ++, big_id);
				stmt.SetParam(index ++, 0);
				stmt.SetParam(index ++, now);
				stmt.SetParam(index ++, now);
				if (stmt.Execute()) {
					relation_id = stmt.GetInsertId();
				} else {
					WLOG << "stmt.execute is failed, insert => " << insert;
				}
			} else {
				WLOG << "stmt.init is failed, insert => " << insert;
			}
		}
		DatabaseManager::ReleaseConnector(db_connector);

		if (relation_id != INVALID_VALUE) {
			if (!MessageModel::GetInstance()->ResetMsgId(relation_id)) {
				WLOG << "resetMsgId is failed";
			}
		}
	} else {
		WLOG << "db connector is NULL";
	}

	return relation_id;
}

} // namespace marz
