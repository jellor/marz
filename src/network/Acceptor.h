#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <vector>
#include "NonCopyable.h"
#include "SocketAcceptor.h"
#include "EventLoopGroup.h"
#include "Handler.h"
#include "IpAddress.h"
#include "Channel.h"

namespace marz {

typedef std::function<void(int fd, const IpAddress& local_address, const IpAddress& peer_address)> AcceptCallback;

class Acceptor : public NonCopyable {
public:
	Acceptor(const std::vector<IpAddress>& ip_address_list, int acceptor_count);
	~Acceptor();

	void Start();
	bool Join();
	int GetThreadCount();

	void SetAcceptCallback(const AcceptCallback& cb) { accept_callback_ = cb; }

private:
	
	void HandleAcceptEvent(int fd, const IpAddress& local_address, const IpAddress& peer_address);

	EventLoopGroup acceptor_group_;
	std::vector<IpAddress> ip_address_list_;
	std::vector<SocketAcceptor*> socket_acceptor_list_;

	AcceptCallback accept_callback_;

};

} // namespace marz

#endif // __ACCEPTOR_H__
