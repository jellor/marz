#include "DatabaseConnector.h"
#include "Logger.h"

namespace marz {

DatabaseConnector::DatabaseConnector(DatabasePool* pool):
pool_(pool),
mysql_(NULL)
{
}

DatabaseConnector::~DatabaseConnector()
{
}

int DatabaseConnector::Init() {
	mysql_ = mysql_init(NULL);
	if (mysql_ == NULL) {
		WLOG << "mysql_init failed";
		return 1;
	}

	my_bool reconnect = true;
	mysql_options(mysql_, MYSQL_OPT_RECONNECT, &reconnect);
	mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

	if (!mysql_real_connect(mysql_, pool_->GetDatabaseServerIp(), pool_->GetDatabaseUserName(),
		pool_->GetDatabasePassword(), pool_->GetDatabaseName(), pool_->GetDatabaseServerPort(), NULL, 0)) {
		WLOG << "mysql_real_connect failed , error " << mysql_error(mysql_);
		return 2;
	}

	return 0;
}

ResultSet* DatabaseConnector::Query(const char* sql) {
	mysql_ping(mysql_);

	if (mysql_real_query(mysql_, sql, strlen(sql))) {
		return NULL;
	}

	MYSQL_RES* res = mysql_store_result(mysql_);
	if (res == NULL) {
		return NULL;
	}

	ResultSet* resultSet = new ResultSet(res);
	return resultSet;
}

bool DatabaseConnector::Update(const char* sql) {
	mysql_ping(mysql_);

	if (mysql_real_query(mysql_, sql, strlen(sql))) {
		return false;
	}

	if (mysql_affected_rows(mysql_) > 0) {
		return true;
	} else {
		return false;
	}
}

char* DatabaseConnector::EscapeString(const char* content, uint32_t content_len) {
	if (content_len > (10240) >> 1) {
		escape_string_[0] = 0;
	} else {
		mysql_real_escape_string(mysql_, escape_string_, content, content_len);
	}

	return escape_string_;
}

uint32_t DatabaseConnector::GetInsertId() {
	return (uint32_t) mysql_insert_id(mysql_);
}

const char* DatabaseConnector::GetPoolName() {
	return pool_->GetPoolName();
}

} // namespace marz
