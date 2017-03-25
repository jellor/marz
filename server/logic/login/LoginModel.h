#ifndef __LOGIN_MODEL_H__
#define __LOGIN_MODEL_H__

#include <string>
#include "Im.Base.pb.h"

namespace marz {

class LoginModel {
public:
	static bool DoLogin(const std::string& name, const std::string& password, Im::Base::UserInfo& user_info);
	static bool UpdateStatus(uint32_t uid, std::string addr, uint8_t status);
};

} // namespace marz

#endif // __LOGIN_MODEL_H__
