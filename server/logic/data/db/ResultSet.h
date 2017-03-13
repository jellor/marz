#ifndef __RESULT_SET_H__
#define __RESULT_SET_H__

#include <string>
#include <map>
#include <mysql.h>

namespace marz {

class ResultSet {
public:
	ResultSet(MYSQL_RES* result);
	~ResultSet();

	int GetRowsNum() const { return num_rows_; }
	int GetFieldsNum() const { return num_fields_; }

	bool Next();
	int GetInt(const char* key);
	char* GetString(const char* key);

private:
	int GetIndex(const char* key);

	MYSQL_RES* result_;
	MYSQL_ROW row_;
	int num_fields_;
	int num_rows_;

	std::map<std::string, int> key_map_;

};

} // namespace marz

#endif // __RESULT_SET_H__
