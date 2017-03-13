#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "Acceptor.h"
#include "Logger.h"
#include "Channel.h"
#include "Logger.h"

namespace marz {

Acceptor::Acceptor(const std::vector<IpAddress>& ip_address_list, int acceptor_count):
acceptor_group_(acceptor_count),
ip_address_list_(ip_address_list),
socket_acceptor_list_(),
accept_callback_(NULL)
{
}

Acceptor::~Acceptor() {
	Join();
	for (int i = 0; i < socket_acceptor_list_.size(); i ++) {
		delete socket_acceptor_list_[i];
	}
	socket_acceptor_list_.clear();	
	DLOG << "Acceptor Destructor Ok";
}

void Acceptor::Start() {
	acceptor_group_.Start();
	for (int i = 0; i < ip_address_list_.size(); i ++) {
		SocketAcceptor* element = new SocketAcceptor(acceptor_group_.Next(), ip_address_list_[i]);
		element->SetAcceptCallback(
			std::bind(&Acceptor::HandleAcceptEvent, this, 
				std::placeholders::_1,
				std::placeholders::_2, 
				std::placeholders::_3));
		element->Listen();
		socket_acceptor_list_.push_back(element);
	}
}

bool Acceptor::Join() {
	return acceptor_group_.Join();
}

int Acceptor::GetThreadCount() {
	return acceptor_group_.Size();
}

void Acceptor::HandleAcceptEvent(int fd, const IpAddress& local_address, const IpAddress& peer_address) {
	accept_callback_(fd, local_address, peer_address);
}

} // namespace marz
