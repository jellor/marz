#ifndef __SOCKET_CONNECTOR_H__
#define __SOCKET_CONNECTOR_H__

#include "EventLoop.h"
#include "Socket.h"
#include "IpAddress.h"
#include "Handle.h"
#include "Channel.h"

namespace marz {

class SocketConnector : public NonCopyable {
	
	typedef std::function<void(const ChannelPtr&)> ConnectCallback;
	typedef std::function<void(void)> ErrorCallback;

public:
	SocketConnector(EventLoop* event_loop, const IpAddress& peer_address, int reconnect_cnt, int reconnect_sec);
	~SocketConnector();

	void Connect();
	void Reconnect();
	void SetConnectCallback(const ConnectCallback& cb) { connect_callback_ = cb; }
	void SetErrorCallback(const ErrorCallback& cb) { error_callback_ = cb; }

private:
	void Run();
	void OnWritable();

	void HandleConnectCallback(const ChannelPtr& channel_ptr) { 
		if (connect_callback_ != NULL) {
			connect_callback_(channel_ptr); 
		}
	}

	void HandleErrorCallback() { 
		if (error_callback_ != NULL) error_callback_(); 
	}

	EventLoop* event_loop_;
	IpAddress peer_address_;
	Socket socket_;
	Handle handle_;
	int reconnect_cnt_;
	int reconnect_sec_;

	ConnectCallback connect_callback_;
	ErrorCallback error_callback_;

};

} // namespace marz

#endif // __SOCKET_CONNECTOR_H__
