#include "PrepareStatement.h"

namespace marz {

PrepareStatement::PrepareStatement():
stmt_(nullptr),
param_bind_(nullptr),
param_count_(0)
{

}

PrepareStatement::~PrepareStatement() {
	if (stmt_ != nullptr) {
		mysql_stmt_close(stmt_);
		stmt_ = nullptr;
	}

	if (param_bind_ != nullptr) {
		delete[] param_bind_;
		param_bind_ = nullptr;
	}
}

bool PrepareStatement::Init(MYSQL* mysql, const std::string& sql) {
	mysql_ping(mysql);

	stmt_ = mysql_stmt_init(mysql);
	if (stmt_ == nullptr) {
		return false;
	}

	if (mysql_stmt_prepare(stmt_, sql.c_str(), sql.size())) {
		return false;
	}

	param_count_ = mysql_stmt_param_count(stmt_);
	if (param_count_ > 0) {
		param_bind_ = new MYSQL_BIND[param_count_];
		if (param_bind_ == nullptr) {
			return false;
		}

		memset(param_bind_, 0, sizeof(MYSQL_BIND) * param_count_);
	}

	return true;
}

void PrepareStatement::SetParam(uint32_t index, int& value) {
	
	if (index >= param_count_) {
		return;
	}

	param_bind_[index].buffer_type = MYSQL_TYPE_LONG;
	param_bind_[index].buffer = &value;
}

void PrepareStatement::SetParam(uint32_t index, uint32_t& value) {
	
	if (index >= param_count_) {
		return;
	}

	param_bind_[index].buffer_type = MYSQL_TYPE_LONG;
	param_bind_[index].buffer = &value;
}

void PrepareStatement::SetParam(uint32_t index, std::string& value) {
	
	if (index >= param_count_) {
		return;
	}

	param_bind_[index].buffer_type = MYSQL_TYPE_STRING;
	param_bind_[index].buffer = (char*) value.c_str();
	param_bind_[index].buffer_length = value.size();
}

void PrepareStatement::SetParam(uint32_t index, const std::string& value) {
	
	if (index >= param_count_) {
		return;
	}

	param_bind_[index].buffer_type = MYSQL_TYPE_STRING;
	param_bind_[index].buffer = (char*) value.c_str();
	param_bind_[index].buffer_length = value.size();
}

bool PrepareStatement::Execute() {	
	
	if (stmt_ == nullptr) {
		return false;
	}

	if (mysql_stmt_bind_param(stmt_, param_bind_)) {
		return false;
	}

	if (mysql_stmt_execute(stmt_)) {
		return false;
	}

	if (mysql_stmt_affected_rows(stmt_) == 0) {
		return false;
	}

	return true;
}

int PrepareStatement::GetInsertId() {
	
	if (stmt_ == nullptr) {
		return -1;
	}
	return mysql_stmt_insert_id(stmt_);
}

} // namespace marz
