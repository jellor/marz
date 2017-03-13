#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include "SocketAcceptor.h"
#include "Logger.h"

namespace marz {

void DefaultAcceptFunc(int fd, const IpAddress& local_address, const IpAddress& peer_address) {
	WLOG << "Should Set AcceptCallback In SocketAccepter";
	WLOG << "Fd => " << fd << " Local Address => " << local_address.ToString() << " Peer Address => " << peer_address.ToString();
}

SocketAcceptor::SocketAcceptor(EventLoop* event_loop, const IpAddress& ip_address):
null_fd_(open("/dev/null", O_CLOEXEC | O_WRONLY)),
event_loop_(event_loop),
ip_address_(ip_address),
socket_(),
handle_(event_loop, socket_.Fd()),
accept_callback_(DefaultAcceptFunc)
{
	// 不要暴露this.
	socket_.SetNoDelay(true);
	socket_.SetReusePort(true);
	socket_.SetReuseAddr(true);
}

SocketAcceptor::~SocketAcceptor() {
	close(null_fd_);
	handle_.Remove();
}

int SocketAcceptor::Listen() {
	event_loop_->DoFunc(std::bind(&SocketAcceptor::ListenInEventLoop, this));// if directly invoke listenInLoop() will happen what.
	return 0;
}

void SocketAcceptor::ListenInEventLoop() {
    handle_.SetReadCallback(std::bind(&SocketAcceptor::OnAccept, this));
	handle_.EnableRead();
	if (socket_.Bind(ip_address_) < 0) {
		DLOG << "Bind Failed";
	}
	if (socket_.Listen() < 0) {
		DLOG << "Listen Failed";
	}
}

void SocketAcceptor::OnAccept() {
	IpAddress peer_address; // ? object in stack
	int fd = socket_.Accept(&peer_address);
	if (fd != -1) {
		accept_callback_(fd, ip_address_, peer_address);
	} else {
		WLOG << "errno => " << Logger::GetError();
		WLOG << "Accept Errno => " << errno;
		if (errno == ENOMEM || errno == EMFILE || errno == ENFILE) {
			errno = 0;
			close(null_fd_);
			fd = ::accept(socket_.Fd(), 0, 0);
			close(fd);
			null_fd_ = open("/dev/null", O_CLOEXEC | O_WRONLY);
		}		
	}
}

} // namespace marz
