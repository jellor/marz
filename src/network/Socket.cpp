#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <assert.h>
#include "Socket.h"


namespace marz {

Socket::Socket():
sockfd_(socket(AF_INET, SOCK_STREAM, 0))
{
	assert(sockfd_ > 0);
	
	SetNonBlock(true);
	int flags = fcntl(sockfd_, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	if (fcntl(sockfd_, F_SETFD, flags) < 0) {

	}
}

Socket::Socket(int sockfd):
sockfd_(sockfd)
{
	SetNonBlock(true);
	int flags = fcntl(sockfd_, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	if (fcntl(sockfd_, F_SETFD, flags) < 0) {

	}
}

Socket::~Socket() {
	if (sockfd_ != -1) {
		::close(sockfd_);
	}
}

int Socket::Bind(const IpAddress& IpAddress) {
	int ret = ::bind(sockfd_, reinterpret_cast<const struct sockaddr*>(&IpAddress.GetAddr()), sizeof(struct sockaddr_in));
	return ret;
}

int Socket::Listen() {
	int ret = ::listen(sockfd_, 1024);
	return ret;
}

int Socket::Accept(IpAddress* IpAddress) {
	struct sockaddr_in addr;
	socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
	int ret = ::accept(sockfd_, (struct sockaddr*)&addr, (socklen_t*)&len);
	if (ret >= 0) {
		IpAddress->SetAddr(addr);
	}
	return ret;
}

int Socket::Connect(const IpAddress& IpAddress) {
	socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
	int ret = ::connect(sockfd_, reinterpret_cast<const struct sockaddr*>(&IpAddress.GetAddr()), len);
	return ret;
}

void Socket::Close() {
	if (sockfd_ != -1) {
		::close(sockfd_);
		sockfd_ = -1;
	}
}

void Socket::SetNonBlock(bool on) {
	int flags = fcntl(sockfd_, F_GETFL, 0);
	if (on) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}
	if (fcntl(sockfd_, F_SETFL, flags) < 0) {

	}
}

void Socket::SetNoDelay(bool on) {
	int optval = on ? 1 : 0;
	int ret = setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&optval), sizeof(optval));
	if (ret < 0) {

	}
}

void Socket::SetReuseAddr(bool on) {
	int optval = on ? 1 : 0;
	int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&optval), sizeof(optval));
	if (ret < 0) {

	}
}

void Socket::SetReusePort(bool on) {
#ifdef SO_REUSEPORT
	int optval = on ? 1 : 0;
	int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<char*>(&optval), sizeof(optval));
	if (ret < 0) {

	}
#else
#error "Platform Not Supported"
#endif
}

void Socket::SetKeepAlive(bool on) {
	int optval = on ? 1 : 0;
	int ret = setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&optval), sizeof(optval));
}

void Socket::SetLinger(bool on, int time_out) {

}

void Socket::ShutdownReceive() {
	::shutdown(sockfd_, SHUT_RD);
}

void Socket::ShutdownSend() {
	::shutdown(sockfd_, SHUT_WR);
}

void Socket::ShutdownBoth() {
	::shutdown(sockfd_, SHUT_RDWR);
}

void Socket::CloseSocket(int sockfd) {
	::close(sockfd);
}

int Socket::GetSocket() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (ret < 0) {

	}
	int flags = fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	if (fcntl(sockfd, F_SETFD, flags) < 0) {

	}
	return sockfd;
}

int Socket::GetError(int sockfd, int* error) {
	socklen_t len = static_cast<socklen_t>(sizeof(error));
	return getsockopt(sockfd, SOL_SOCKET, SO_ERROR, error, &len);
}


bool Socket::GetLocalAddress(int sockfd, IpAddress* ip_address) {
	struct sockaddr_in addr;
	socklen_t address_len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
	int ret = getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&addr), &address_len);
	if (ret == 0) {
		ip_address->SetAddr(addr);
		return true;
	} else {
		return false;
	}
}

bool Socket::GetPeerAddress(int sockfd, IpAddress* ip_address) {
	struct sockaddr_in addr;
	socklen_t address_len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
	int ret = getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&addr), &address_len);
	if (ret == 0) {
		ip_address->SetAddr(addr);
		return true;
	} else {
		return false;
	}
}

bool Socket::IsSelfConnect(int sockfd) {
	IpAddress local_address;
	IpAddress peer_address;
	if (GetLocalAddress(sockfd, &local_address) && GetPeerAddress(sockfd, &peer_address)) {
		if (local_address == peer_address) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

} // namespace marz
