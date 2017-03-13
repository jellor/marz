#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include <vector>
#include "NonCopyable.h"
#include "IpAddress.h"
#include "Acceptor.h"
#include "Connector.h"
#include "Handler.h"

namespace marz {

class Bootstrap : public NonCopyable {

	typedef std::function<bool(int fd, const IpAddress& local_address, const IpAddress& peer_address)> FilterCallback;
	typedef std::function<void(const ChannelPtr&)> EventCallback;
	typedef std::function<Handler* (const ChannelPtr&)> HanderCreator;

public:
	Bootstrap();
	~Bootstrap();

	void SetFilterCallback(const FilterCallback& cb) { filter_callback_  = cb; }
	void SetConnectCallback(const EventCallback& cb) { connect_callback_ = cb; }
	void SetReadCallback(const EventCallback& cb) 	 { read_callback_    = cb; }
	void SetWriteCallback(const EventCallback& cb)   { write_callback_ = cb;   }
	void SetCloseCallback(const EventCallback& cb)   { close_callback_ = cb;   }
	void SetErrorCallback(const EventCallback& cb)   { error_callback_ = cb;   }

	void Start();
	void Wait();

	void SetMainThread(const std::vector<IpAddress>& listen_address_list, const std::vector<IpAddress>&  connect_address_list) {
		listen_addrs_in_main_thread_ = listen_address_list;
		connect_addrs_in_main_thread_ = connect_address_list;
	}

	void SetAcceptThread(const std::vector<IpAddress>& address_list, int accept_thread_cnt) {
		listen_addrs_in_accept_thread_ = address_list;
		assert(accept_thread_cnt > 0);
		accept_thread_cnt_ = accept_thread_cnt;
	}

	void SetIoThread(int io_thread_cnt) {
		assert(io_thread_cnt > 0);
		io_group_ = new EventLoopGroup(io_thread_cnt);
	}

	void SetConnectThread(const std::vector<IpAddress>& address_list) {
		connect_addrs_in_connect_thread_ = address_list;
	}

	void SetHandlerCreator(const HanderCreator& handler_creator) { 
		handler_creator_ = handler_creator;
	}

private:
	void StartListenMainThread();
	void StartConnectMainThread();
	void HandleConnectEvent(const ChannelPtr& channel_ptr);

	bool HandleFilter(int fd, const IpAddress& local_address, const IpAddress& peer_address);
	void HandleAccept(int fd, const IpAddress& local_address, const IpAddress& peer_address);
	void RunInEventLoop(EventLoop* event_loop, int fd, const IpAddress& local_address, const IpAddress& peer_address);
	void HandleConnect(const ChannelPtr& channel_ptr);
	void HandleRead(const ChannelPtr& channel_ptr);
	void HandleClose(const ChannelPtr& channel_ptr);
	void HandleWrite(const ChannelPtr& channel_ptr);
	void HandleError(const ChannelPtr& channel_ptr);

	EventLoop* main_loop_;
	EventLoopGroup* io_group_;
	std::vector<SocketAcceptor*> socket_acceptor_list_;
	std::vector<SocketConnector*> socket_connector_list_;
	Acceptor*  acceptor_;
	Connector* connector_;

	std::vector<IpAddress> listen_addrs_in_main_thread_;
	std::vector<IpAddress> connect_addrs_in_main_thread_;
	std::vector<IpAddress> connect_addrs_in_connect_thread_;
	std::vector<IpAddress> listen_addrs_in_accept_thread_;
	int accept_thread_cnt_;
	int io_thread_cnt_;
	int reconnect_cnt_;
	int reconnect_sec_;

	FilterCallback filter_callback_;
	EventCallback accept_callback_;
	EventCallback connect_callback_;
	EventCallback read_callback_;
	EventCallback write_callback_;
	EventCallback close_callback_;
	EventCallback error_callback_;

	HanderCreator handler_creator_;

};


} // namesapce marz

#endif // __BOOTSTRAP_H__
