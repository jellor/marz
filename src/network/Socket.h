#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "NonCopyable.h"
#include "IpAddress.h"

namespace marz {

class Socket : public NonCopyable {
public:
	Socket();
	Socket(int sockfd);
	~Socket();

	void Reset() {
		Close();
		sockfd_ = -1;
	}

	void Reset(int sockfd) {
		Close();
		sockfd_ = sockfd;
	}

	int Fd() const { return sockfd_; }
	void SetFd(int sockfd) { sockfd_ = sockfd; }
	bool Available() const { return sockfd_ != -1 ? true : false; }
	
	int	Bind(const IpAddress& IpAddress);
	int Listen();
	int Accept(IpAddress* IpAddress);
	int Connect(const IpAddress& IpAddress);
	void Close();

	void SetNonBlock(bool on);
	void SetNoDelay(bool on);
	void SetReuseAddr(bool on);
	void SetReusePort(bool on);
	void SetKeepAlive(bool on);
	void SetLinger(bool on, int time_out);
	
	void ShutdownReceive();
	void ShutdownSend();
	void ShutdownBoth();

	static void CloseSocket(int sockfd);
	static int GetSocket();
	static int GetError(int sockfd, int* error);
	static bool GetLocalAddress(int sockfd, IpAddress* IpAddress);
	static bool GetPeerAddress(int sockfd, IpAddress* IpAddress);
	static bool IsSelfConnect(int sockfd);

private:
	int sockfd_;
	
};

} // namespace marz

#endif // __SOCKET_H__
