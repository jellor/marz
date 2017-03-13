#include "Bootstrap.h"

namespace marz {

Bootstrap::Bootstrap():
main_loop_(NULL),
io_group_(NULL),
socket_acceptor_list_(),
socket_connector_list_(),
acceptor_(NULL),
connector_(NULL),
listen_addrs_in_main_thread_(),
connect_addrs_in_main_thread_(),
connect_addrs_in_connect_thread_(),
listen_addrs_in_accept_thread_(),
accept_thread_cnt_(0),
io_thread_cnt_(0),
reconnect_cnt_(3),
reconnect_sec_(1),
filter_callback_(NULL),
connect_callback_(NULL),
read_callback_(NULL),
write_callback_(NULL),
close_callback_(NULL),
error_callback_(NULL),
handler_creator_(NULL)
{
	// 不要暴露this.
	main_loop_ = new EventLoop();
}

Bootstrap::~Bootstrap() {
	if (acceptor_ != NULL) {
		acceptor_->Join();
		delete acceptor_;
	}
	if (connector_ != NULL) {
		connector_->Join();
		delete connector_;
	}
	for (int i = 0; i < socket_acceptor_list_.size(); i ++) {
		delete socket_acceptor_list_[i];
	}
	for (int i = 0; i < socket_connector_list_.size(); i ++) {
		delete socket_connector_list_[i];
	}
	if (io_group_ != NULL) {
		delete io_group_;
	}
	delete main_loop_;
	DLOG << "Bootstrap Destructor ...";
}

void Bootstrap::StartListenMainThread() {
	for (int i = 0; i < listen_addrs_in_main_thread_.size(); i ++) {
		SocketAcceptor* element = new SocketAcceptor(main_loop_, listen_addrs_in_main_thread_[i]);
		element->SetAcceptCallback(
			std::bind(&Bootstrap::HandleAccept, this, 
				std::placeholders::_1,
				std::placeholders::_2, 
				std::placeholders::_3));
		element->Listen();
		socket_acceptor_list_.push_back(element);
	}
}

void Bootstrap::StartConnectMainThread() {
	for (int i = 0; i < connect_addrs_in_main_thread_.size(); i ++) {
		SocketConnector* element = new SocketConnector(main_loop_, connect_addrs_in_main_thread_[i], reconnect_cnt_, reconnect_sec_);
		element->SetConnectCallback(std::bind(&Bootstrap::HandleConnectEvent, this, std::placeholders::_1));
		element->Connect();
		//element->setConnectCallback(std::bind(&Connector::handleConnectEvent, this, std::placeholders::_1));
		socket_connector_list_.push_back(element);
	}
}

void Bootstrap::HandleConnectEvent(const ChannelPtr& channel_ptr) {
	HandleConnect(channel_ptr);
	// channel_ptr->setConnectCallback(std::bind(&Bootstrap::handleConnect, this, channel_ptr));
	channel_ptr->SetReadCallback(std::bind(&Bootstrap::HandleRead,   this, std::placeholders::_1));
	channel_ptr->SetCloseCallback(std::bind(&Bootstrap::HandleClose, this, std::placeholders::_1));
	channel_ptr->SetWriteCallback(std::bind(&Bootstrap::HandleWrite, this, std::placeholders::_1));
	channel_ptr->SetErrorCallback(std::bind(&Bootstrap::HandleError, this, std::placeholders::_1));
	// channel_ptr->getHandler()->enableRead();
}

void Bootstrap::Start() {
	if (io_group_ != NULL) {
		io_group_->Start();
	}

	if (accept_thread_cnt_ > 0 && listen_addrs_in_accept_thread_.size() > 0) {
		acceptor_ = new Acceptor(listen_addrs_in_accept_thread_, accept_thread_cnt_);
		if (acceptor_ == NULL) {}

		acceptor_->SetAcceptCallback(
			std::bind(&Bootstrap::HandleAccept, this, 
				std::placeholders::_1, 
				std::placeholders::_2, 
				std::placeholders::_3));
		
		acceptor_->Start();
	}

	if (connect_addrs_in_connect_thread_.size() > 0) {
		connector_ = new Connector(connect_addrs_in_connect_thread_);
		if (connector_ != NULL) { }

		connector_->SetConnectCallback(std::bind(&Bootstrap::HandleConnect, this, std::placeholders::_1));
		connector_->SetReadCallback(std::bind(&Bootstrap::HandleRead, this, std::placeholders::_1));
		connector_->SetCloseCallback(std::bind(&Bootstrap::HandleClose, this, std::placeholders::_1));
		connector_->SetWriteCallback(std::bind(&Bootstrap::HandleWrite, this, std::placeholders::_1));
		connector_->SetErrorCallback(std::bind(&Bootstrap::HandleError, this, std::placeholders::_1));

		connector_->Start(reconnect_cnt_, reconnect_sec_);
	}

	StartListenMainThread();
	StartConnectMainThread();
	
	main_loop_->Start();
}

void Bootstrap::Wait() {
	io_group_->Join();
	acceptor_->Join();
	connector_->Join();
	DLOG << "Join";
}

bool Bootstrap::HandleFilter(int fd, const IpAddress& local_address, const IpAddress& peer_address) {
	DLOG << "Handle Filter";
	if (filter_callback_ != NULL) {
		return filter_callback_(fd, local_address, peer_address);
	} else {
		return true;
	}
}

void Bootstrap::HandleAccept(int fd, const IpAddress& local_address, const IpAddress& peer_address) {
	if (filter_callback_ != NULL) {
		if (!HandleFilter(fd, local_address, peer_address)) {
			Socket::CloseSocket(fd);
			DLOG << "Peer Address => " << peer_address.ToString() << " Are Refused";
			return;
		}
	}

	DLOG << "Fd => " << fd << " Local Address => " << local_address.ToString() 
		<< " Peer Address => " << peer_address.ToString();

	EventLoop* event_loop = NULL;
	if (io_group_ != NULL) {
		event_loop = io_group_->Next(); 
	} else {
		event_loop = main_loop_;
	}
	assert(event_loop != NULL);
	event_loop->DoFunc(std::bind(&Bootstrap::RunInEventLoop, this, event_loop, fd, local_address, peer_address));
}

void Bootstrap::RunInEventLoop(EventLoop* event_loop, int fd, const IpAddress& local_address, const IpAddress& peer_address) {
	ChannelPtr channel_ptr(new Channel(event_loop, fd, local_address, peer_address));

	HandleConnect(channel_ptr);
	channel_ptr->SetReadCallback(std::bind(&Bootstrap::HandleRead, this, std::placeholders::_1));
	channel_ptr->SetCloseCallback(std::bind(&Bootstrap::HandleClose, this, std::placeholders::_1));
	channel_ptr->SetWriteCallback(std::bind(&Bootstrap::HandleWrite, this, std::placeholders::_1));
	channel_ptr->SetErrorCallback(std::bind(&Bootstrap::HandleError, this, std::placeholders::_1));

	channel_ptr->GetHandle()->EnableRead();
}

void Bootstrap::HandleConnect(const ChannelPtr& channel_ptr) {
	DLOG << "Handle Connect";
	if (connect_callback_ != NULL) {
		connect_callback_(channel_ptr);
	}

	if (handler_creator_ != NULL) {
		Handler* handler = handler_creator_(channel_ptr);
		if (handler != NULL) {
			channel_ptr->SetHandler(handler);
			handler->OnActive(channel_ptr);
		}
	}	
}

void Bootstrap::HandleRead(const ChannelPtr& channel_ptr) {
	DLOG << "Handle Read";
	if (read_callback_ != NULL) {
		read_callback_(channel_ptr);
	}
	
	Handler* handler = channel_ptr->GetHandler();
	if (handler!= NULL) {
		handler->OnReceive(channel_ptr);
	}
}

void Bootstrap::HandleClose(const ChannelPtr& channel_ptr) {
	DLOG << "Handle Close";
	if (close_callback_ != NULL) {
		close_callback_(channel_ptr);
	}

	Handler* handler = channel_ptr->GetHandler();
	if (handler != NULL) {
		handler->OnInactive(channel_ptr);
		delete handler;
	}
}

void Bootstrap::HandleWrite(const ChannelPtr& channel_ptr) {
	DLOG << "Handle Write";
	if (write_callback_ != NULL) {
		write_callback_(channel_ptr);
	}
}

void Bootstrap::HandleError(const ChannelPtr& channel_ptr) {
	DLOG << "Handle Error";
	if (error_callback_ != NULL) {
		error_callback_(channel_ptr);
	}

	Handler* handler = channel_ptr->GetHandler();
	if (handler != NULL) {
		handler->OnError(channel_ptr);
		delete handler;
	}
}

} // namespace marz
