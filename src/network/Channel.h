#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <memory>
#include <string>
#include <assert.h>
#include "RingBuffer.h"
#include "IpAddress.h"
#include "EventLoop.h"
#include "Handle.h"
#include "Socket.h"

namespace marz {

class Handler;

class Channel : public std::enable_shared_from_this<Channel> {

	typedef std::function<void(const std::shared_ptr<Channel>&)> EventCallback;

public:
	Channel(EventLoop* event_loop, int sockfd, const IpAddress& local_address, const IpAddress& peer_address);
	~Channel();

	void SetReadCallback(const EventCallback& cb) 	 { read_callback_    = cb; }
	void SetWriteCallback(const EventCallback& cb)   { write_callback_ = cb;   }
	void SetCloseCallback(const EventCallback& cb)   { close_callback_ = cb;   }
	void SetErrorCallback(const EventCallback& cb)   { error_callback_ = cb;   }

	bool IsOpen() const { return opened_; }

	void EnableRead() {
		assert(opened_ == true);
		handle_.EnableRead();
	}

	void EnableWrite() {
		assert(opened_ == true);
		handle_.EnableWrite();
	}

	void DisableRead() {
		assert(opened_ == true);
		handle_.DisableRead();
	}

	void DisableWrite() {
		assert(opened_ == true);
		handle_.DisableWrite();
	}

	void DisableBoth() {
		assert(opened_ == true);
		handle_.DisableRead();
		handle_.DisableWrite();
	}

	void ShutdownReceive();
	void ShutdownSend();
	void Close();

	std::string ToString() const;

	void Send(const RingBuffer* buffer);
	void Send(const char* data, unsigned int size);

	EventLoop* GetEventLoop() const { return event_loop_; }
	Socket*  GetSocket() 	  { return &socket_; 	}
	Handle* GetHandle()  	  { return &handle_;   }

	const IpAddress& GetLocalAddress()  const { return local_address_; }
	const IpAddress& GetPeerAddress()   const { return peer_address_;  }

	RingBuffer* GetMutableOutBuffer() const { return const_cast<RingBuffer*>(&out_buffer_); }
	RingBuffer* GetMutableInBuffer()  const { return const_cast<RingBuffer*>(&in_buffer_);  }
	const RingBuffer* GetOutBuffer() const { return &out_buffer_; }
	const RingBuffer* GetInBuffer()  const { return &in_buffer_;  }


	void SetHandler(Handler* handler) {
		handler_ = handler;
	}

	Handler* GetHandler() const {
		return handler_;
	}

private:
	void ShutdownReceiveInEventLoop();
	void ShutdownSendInEventLoop();
	void CloseInEventLoop();
	
	int Read(char* data, unsigned int size);
	int Write(const char* data, unsigned int size);
	void SendBufferInEventLoop(const RingBuffer* buffer);
	void SendInEventLoop(const char* data, unsigned int size);
	void HandleReadEvent();
	void HandleWriteEvent();

	EventLoop* event_loop_;

	Socket socket_;
	IpAddress local_address_;
	IpAddress peer_address_;
	Handle handle_;
	RingBuffer out_buffer_;
	RingBuffer in_buffer_;

	std::atomic<bool> opened_;

	EventCallback read_callback_;
	EventCallback write_callback_;
	EventCallback close_callback_;
	EventCallback error_callback_;

	Handler* handler_;
};

typedef std::shared_ptr<Channel> ChannelPtr;

} // namespace marz

#endif // __CHANNEL_H__
