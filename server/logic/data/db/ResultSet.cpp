#include "ResultSet.h"

namespace marz {

ResultSet::ResultSet(MYSQL_RES* result):
result_(result),
num_fields_(mysql_num_fields(result_)),
num_rows_(mysql_num_rows(result_))
{
	MYSQL_FIELD* fields = mysql_fetch_fields(result_);
	for (int i = 0; i < num_fields_; i ++) {
		key_map_.insert(std::make_pair(fields[i].name, i));
	}
}

ResultSet::~ResultSet() {
	if (result_ != nullptr) {
		mysql_free_result(result_);
		result_ = nullptr;
	}
}

bool ResultSet::Next() {
	row_ = mysql_fetch_row(result_);
	if (row_ != nullptr) {
		return true;
	} else {
		return false;
	}
}

int ResultSet::GetInt(const char* key) {
	int index = GetIndex(key);
	if (index < 0) {
		return -1;
	} else {
		return atoi(row_[index]);
	}
}

char* ResultSet::GetString(const char* key) {
	int index = GetIndex(key);
	if (index < 0) {
		return nullptr;
	} else {
		return row_[index];
	}
}

int ResultSet::GetIndex(const char* key) {
	std::map<std::string, int>::iterator it = key_map_.find(key);
	if (it != key_map_.end()) {
		return it->second;
	} else {
		return -1;
	}
}

} // namespace marz
