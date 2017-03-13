#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <set>
#include "EventLoop.h"
#include "IpAddress.h"
#include "Thread.h"
#include "SocketConnector.h"

namespace marz {

class Connector : public NonCopyable {
	typedef std::function<void(const ChannelPtr&)> EventCallback;
public:
	Connector(const std::vector<IpAddress>& ip_address_list);
	~Connector();

	void Start(int reconnect_cnt, int reconnect_sec);
	void Connect(const IpAddress& remote_address, int reconnect_cnt, int reconnect_sec);
	void Join();
	bool IsStarted() const { return started_; }
	void SetConnectCallback(const EventCallback& cb) { connect_callback_ = cb; }
	void SetReadCallback(const EventCallback& cb) 	 { read_callback_    = cb; }
	void SetWriteCallback(const EventCallback& cb)   { write_callback_ = cb;   }
	void SetCloseCallback(const EventCallback& cb)   { close_callback_ = cb;   }
	void SetErrorCallback(const EventCallback& cb)   { error_callback_ = cb;   }

private:
	void HandleConnect(const ChannelPtr& channel_ptr) {
		if (connect_callback_ != NULL) connect_callback_(channel_ptr);
	}

	void HandleRead(const ChannelPtr& channel_ptr) {
		if (read_callback_ != NULL) read_callback_(channel_ptr);
	}

	void HandleClose(const ChannelPtr& channel_ptr) {
		if (close_callback_ != NULL) close_callback_(channel_ptr);
	}

	void HandleWrite(const ChannelPtr& channel_ptr) {
		if (write_callback_ != NULL) write_callback_(channel_ptr);
	}

	void HandleError(const ChannelPtr& channel_ptr) {
		if (error_callback_ != NULL) error_callback_(channel_ptr);
	}
	void RunInThread();
	void HandleConnectEvent(const ChannelPtr& channel_ptr);

	std::atomic<bool> started_;
	EventLoop* event_loop_;
	std::vector<IpAddress> ip_address_list_;
	std::vector<SocketConnector*> socket_connector_list_;

	Thread thread_;
	EventCallback connect_callback_;
	EventCallback read_callback_;
	EventCallback write_callback_;
	EventCallback close_callback_;
	EventCallback error_callback_;
};

} // namespace marz

#endif // __CONNECTOR_H__
