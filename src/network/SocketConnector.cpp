#include <errno.h>
#include "SocketConnector.h"
#include "Logger.h"
#include "Timestamp.h"

namespace marz {

SocketConnector::SocketConnector(EventLoop* event_loop, const IpAddress& peer_address, int reconnect_cnt, int reconnect_sec):
event_loop_(event_loop),
peer_address_(peer_address),
socket_(),
handle_(event_loop_, socket_.Fd()),
reconnect_cnt_(reconnect_cnt),
reconnect_sec_(reconnect_sec),
connect_callback_(NULL),
error_callback_(NULL)
{
	socket_.SetNoDelay(true);
}

SocketConnector::~SocketConnector() {
	
}

void SocketConnector::Connect() {
	event_loop_->DoFunc(std::bind(&SocketConnector::Run, this));
}

void SocketConnector::Reconnect() {
	if (reconnect_cnt_ -- > 0) {
		handle_.Reset();
		socket_.Close();
		handle_.Reset(event_loop_, socket_.Fd());
		socket_.SetNoDelay(true);
		Connect();
	} else {
		HandleErrorCallback();
	}
}

void SocketConnector::Run() {

	int ret = socket_.Connect(peer_address_);
	int err = errno;
	if (ret == -1) {
		int sock_err;

		DLOG << "errno " << Logger::GetError();

		if (err == EINPROGRESS || err == EINTR) {
			errno = 0;
			handle_.SetWriteCallback(std::bind(&SocketConnector::OnWritable, this));
			handle_.EnableWrite();
		} else {
			DLOG << "Connect errno = > " << err;
			HandleErrorCallback();
		}
	} 
	
	if (ret == 0) {
		handle_.DisableWrite();
		IpAddress local_address;
		DLOG << "Fd => " << socket_.Fd() << "Local Address " << local_address.ToString() << " Peer  Address " << peer_address_.ToString();
		Socket::GetPeerAddress(socket_.Fd(), &local_address);

		ChannelPtr channel_ptr(new Channel(event_loop_, socket_.Fd(), local_address, peer_address_));
		HandleConnectCallback(channel_ptr);
	}

}

void SocketConnector::OnWritable() {
	IpAddress peer_address;

	int err = errno;
	if (Socket::GetPeerAddress(socket_.Fd(), &peer_address)) {
		handle_.DisableWrite();
		IpAddress local_address;
		Socket::GetLocalAddress(socket_.Fd(), &local_address);
		DLOG << "Fd => " << socket_.Fd() << " Local Address " << local_address.ToString() << " Peer  Address " << peer_address.ToString();

		if (!Socket::IsSelfConnect(socket_.Fd())) {

			ChannelPtr channel_ptr(new Channel(event_loop_, socket_.Fd(), local_address, peer_address));
			HandleConnectCallback(channel_ptr);
		} else {
			int error;
			if (Socket::GetError(socket_.Fd(), &error) != 0 || error != 0) {
				WLOG << Logger::GetError();
			}
			WLOG << "Connect To Self";
		}
	} else {
		DLOG << "errno " << Logger::GetError();
		if (err == ETIMEDOUT || err == ECONNRESET || err == ECONNREFUSED || err == EINTR) {
			event_loop_->AddTimer(std::bind(&SocketConnector::Reconnect, this), Timestamp::Now().Macrosecond(), reconnect_sec_, 1);
		} else {
			//assert(false);
			DLOG << "Fd => " << socket_.Fd() << " Peer  Address " << peer_address.ToString();
			HandleErrorCallback();
			handle_.Remove();
		}
	}
}

} // namespace marz
