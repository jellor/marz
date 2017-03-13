#ifndef __SOCKET_ACCEPTOR_H__
#define __SOCKET_ACCEPTOR_H__

#include "NonCopyable.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Handle.h"
#include "IpAddress.h"

namespace marz {

class SocketAcceptor : public NonCopyable {

	typedef std::function<void(int fd, const IpAddress& local_address, const IpAddress& peer_address)> AcceptCallback;
	
public:
	SocketAcceptor(EventLoop* event_loop, const IpAddress& ip_address);
	~SocketAcceptor();

	void SetAcceptCallback(const AcceptCallback& cb) { accept_callback_ =  cb; }
	int Listen();
	
private:
	void ListenInEventLoop();
	void OnAccept();

	int null_fd_;
	EventLoop* event_loop_;
	IpAddress ip_address_;
	Socket socket_;
	Handle handle_;
	AcceptCallback accept_callback_;	

};

} // namespace marz

#endif // __SOCKET_ACCEPTOR_H__
