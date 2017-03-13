#ifndef __SESSION_MODEL_H__
#define __SESSION_MODEL_H__

#include "Im.Base.pb.h"
using namespace Im;

#include <list>

namespace marz {

class SessionModel {
public:
	virtual ~SessionModel();
	static SessionModel *GetInstance();

	void GetRecentSession(uint32_t user_id, uint32_t last_time, std::list<Im::Base::ContactSessionInfo>& contact_list);
	uint32_t GetSessionId(uint32_t user_id, uint32_t peer_id, uint32_t type, bool isAll);
	uint32_t AddSession(uint32_t user_id, uint32_t peer_id, uint32_t type);
	bool UpdateSession(uint32_t session_id, uint32_t update_time);
	bool RemoveSession(uint32_t session_id);

private:
	SessionModel();

	void FillSessionMsg(uint32_t user_id, std::list<Im::Base::ContactSessionInfo>& contact_list);
	
	static SessionModel *instance_;

};

} // namespace marz

#endif // __SESSION_MODEL_H_
