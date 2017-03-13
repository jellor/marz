#ifndef __PREPARE_STATEMENT_H__
#define __PREPARE_STATEMENT_H__

#include <string>
#include <mysql.h>

namespace marz {

class PrepareStatement {
public:
	PrepareStatement();
	virtual ~PrepareStatement();

	bool Init(MYSQL* mysql, const std::string& sql);
	void SetParam(uint32_t index, int& value);
	void SetParam(uint32_t index, uint32_t& value);
	void SetParam(uint32_t index, std::string& value);
	void SetParam(uint32_t index, const std::string& value);

	bool Execute();
	int GetInsertId();

private:
	MYSQL_STMT* stmt_;
	MYSQL_BIND* param_bind_;
	uint32_t param_count_;
};

} // namespace marz

#endif // __PREPARE_STATEMENT_H__
