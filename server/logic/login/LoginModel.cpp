#include "LoginModel.h"
#include "CacheManager.h"
#include "DatabaseManager.h"
#include "Logger.h"
#include "Md5.h"

namespace marz {

bool LoginModel::DoLogin(const std::string& name, const std::string& password, Im::Base::UserInfo& user_info) {
	bool ret = false;

	// SQL注入
	std::string	sql = "SELECT * FROM ImUser WHERE status=0 AND name=" + name;
	
	DatabaseConnector* db_connector = DatabaseManager::GetConnector("database_master");
	if (db_connector != NULL) {
		ResultSet *result = db_connector->Query(sql);
		DatabaseManager::ReleaseConnector(db_connector);

		if (result != NULL) {
			uint32_t user_id;
			uint8_t sex;
			std::string name;
			std::string pwd;
			std::string salt;
			std::string domain;
			std::string nick;
			std::string phone;
			std::string email;
			std::string avatar;
			uint8_t status;

			while (result->Next()) {
				user_id = result->GetInt("id");
				pwd = result->GetString("pwd");
				salt = result->GetString("salt");
				sex = result->GetInt("sex");
				nick = result->GetString("nick");
				avatar = result->GetString("avatar");
				phone = result->GetString("phone");
				email = result->GetString("email");
				name = result->GetString("name");
				domain = result->GetString("domain");
				status = result->GetInt("status");
			}
			delete result;

			std::string in_pass = password + salt;
			char str_md5[33];
			Md5::Calculate(in_pass.c_str(), in_pass.length(), str_md5);
			std::string out_pass(str_md5);

			DLOG << "out_pass => " << out_pass << " pwd => " << pwd;
			if (out_pass == pwd) {
				ret = true;
				user_info.set_user_id(user_id);
				user_info.set_nick(nick);
				user_info.set_sex(sex);
				user_info.set_avatar(avatar);
				user_info.set_phone(phone);
				user_info.set_email(email);
				user_info.set_name(name);
				user_info.set_domain(domain);
				user_info.set_status(status);
			} else {
				WLOG << "out_pass != pwd";
			}
			
		} else {
			WLOG << sql << " Failed";
		}
	} else {
		WLOG << "db connector is NULL";
	}

	return ret;
}

} // namespace marz
