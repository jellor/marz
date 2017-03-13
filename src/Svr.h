#ifndef __SVR_H__
#define __SVR_H__

#include "NonCopyable.h"
#include "Bootstrap.h"

namespace marz {

class Svr : public NonCopyable {
public:
	Svr(): bootstrap_()
	{
		DLOG << "Svr Constructor ......";
	}

	virtual ~Svr() {
		bootstrap_.Wait();
		DLOG << "Svr Destructor ......";
	}

	virtual Handler* CreateHandler(const ChannelPtr& channel_ptr) = 0;

	void Start() {
		bootstrap_.SetHandlerCreator(std::bind(&Svr::CreateHandler, this, std::placeholders::_1));
		bootstrap_.Start();
		bootstrap_.Wait();
	}

	void SetMainThread(const std::vector<IpAddress>& listen_address_list, const std::vector<IpAddress>&  connect_address_list) {
		bootstrap_.SetMainThread(listen_address_list, connect_address_list);
	}

	void SetAcceptThread(const std::vector<IpAddress>& address_list, int accept_thread_cnt) {
		bootstrap_.SetAcceptThread(address_list, accept_thread_cnt);
	}

	void SetIoThread(int io_thread_cnt) {
		bootstrap_.SetIoThread(io_thread_cnt);
	}

	void SetConnectThread(const std::vector<IpAddress>& address_list) {
		SetConnectThread(address_list);
	}

private:

	Bootstrap bootstrap_;
};

} // namespace marz

#endif // __SVR_H__