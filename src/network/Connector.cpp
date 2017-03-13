#include <errno.h>
#include "Connector.h"

namespace marz {

Connector::Connector(const std::vector<IpAddress>& ip_address_list):
started_(false),
event_loop_(NULL),
ip_address_list_(ip_address_list),
socket_connector_list_(),
thread_(),
connect_callback_(NULL),
read_callback_(NULL),
write_callback_(NULL),
close_callback_(NULL),
error_callback_(NULL)
{
	
}

Connector::~Connector() {
	thread_.Join();
	event_loop_->Stop();

	while (started_ == true) {}
	
	for (int i = 0; i < socket_connector_list_.size(); i ++) {
		delete socket_connector_list_[i];
	}
	socket_connector_list_.clear();
}

void Connector::Start(int reconnect_cnt, int reconnect_sec) {
	thread_.SetThreadFunc(std::bind(&Connector::RunInThread, this));
	thread_.SetThreadName("Connector Thread");
	thread_.Start();

	while (started_ == false) {}
	for (int i = 0; i < ip_address_list_.size(); i ++) {
		SocketConnector* element = new SocketConnector(event_loop_, ip_address_list_[i], reconnect_cnt, reconnect_sec);
		element->SetConnectCallback(std::bind(&Connector::HandleConnectEvent, this, std::placeholders::_1));
		element->Connect();
		//element->setConnectCallback(std::bind(&Connector::handleConnectEvent, this, std::placeholders::_1));
		socket_connector_list_.push_back(element);
	}
}

void Connector::Connect(const IpAddress& remote_address, int reconnect_cnt, int reconnect_sec) {
	SocketConnector* element = new SocketConnector(event_loop_, remote_address, reconnect_cnt, reconnect_sec);
	element->SetConnectCallback(std::bind(&Connector::HandleConnectEvent, this, std::placeholders::_1));
	element->Connect();
	//element->setConnectCallback(std::bind(&Connector::handleConnectEvent, this, std::placeholders::_1));
	socket_connector_list_.push_back(element);
}

void Connector::Join() {
	thread_.Join();
}

void Connector::HandleConnectEvent(const ChannelPtr& channel_ptr) {

	HandleConnect(channel_ptr);
	// channel_ptr->setConnectCallback(std::bind(&Connector::handleConnect, this, channel_ptr));
	channel_ptr->SetReadCallback(std::bind(&Connector::HandleRead,   this, std::placeholders::_1));
	channel_ptr->SetCloseCallback(std::bind(&Connector::HandleClose, this, std::placeholders::_1));
	channel_ptr->SetWriteCallback(std::bind(&Connector::HandleWrite, this, std::placeholders::_1));
	channel_ptr->SetErrorCallback(std::bind(&Connector::HandleError, this, std::placeholders::_1));

	// channel_ptr->getHandler()->enableRead();
}

void Connector::RunInThread() {
	event_loop_ = new EventLoop();
	started_    = true;

	event_loop_->Start();
	delete event_loop_;
	
	event_loop_	= NULL;
	started_ 	= false;
}

} // namespace marz
