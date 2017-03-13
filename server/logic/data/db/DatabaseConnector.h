#ifndef __DATABASE_CONNECTOR_H__
#define __DATABASE_CONNECTOR_H__

#include "DatabasePool.h"
#include "ResultSet.h"

namespace marz {

class DatabaseConnector {
public:
	DatabaseConnector(DatabasePool* pool);
	virtual ~DatabaseConnector();

	int Init();

	ResultSet* Query(const char* sql);
	bool Update(const char* sql);
	ResultSet* Query(const std::string& sql) { return Query(sql.c_str()); }
	bool Update(const std::string& sql) { return Update(sql.c_str()); }

	char* EscapeString(const char* content, uint32_t content_len);

	uint32_t GetInsertId();
	const char* GetPoolName();
	MYSQL* GetMysql() const { return mysql_; }

private:
	DatabasePool* pool_;
	MYSQL* mysql_;
	char escape_string_[10240 - 1];

};

} // namespace marz

#endif // __DATABASE_CONNECTOR_H__
