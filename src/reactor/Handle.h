#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <functional>
#include "EventLoop.h"
#include "NonCopyable.h"
 
namespace marz {

class EventLoop;

typedef enum {
	    NEW = 0,
	    ADD = 1,
	    DEL = 2,
} HandleStatus;

class Handle : public NonCopyable {

	typedef std::function<void()> EventCallback;

public:
	Handle(EventLoop* event_loop, int fd = -1);
	~Handle();

	void SetReadCallback (const EventCallback& cb) { read_callback_  = cb; }
	void SetWriteCallback(const EventCallback& cb) { write_callback_ = cb; }
	void SetCloseCallback(const EventCallback& cb) { close_callback_ = cb; }
	void SetErrorCallback(const EventCallback& cb) { error_callback_ = cb; }
	void SetReadCallback (EventCallback&& cb) { read_callback_  = std::move(cb); }
	void SetWriteCallback(EventCallback&& cb) { write_callback_ = std::move(cb); }
	void SetCloseCallback(EventCallback&& cb) { close_callback_ = std::move(cb); }
	void SetErrorCallback(EventCallback&& cb) { error_callback_ = std::move(cb); }

	bool IsReadable() const { return readable_; }
	bool IsWritable() const { return writable_; }
	void EnableRead()   { if (readable_) return ; readable_ = true;  Update(); }
	void DisableRead()  { if (!readable_)return ; readable_ = false; Update(); }
	void EnableWrite()  { if (writable_) return ; writable_ = true;  Update(); }
	void DisableWrite() { if (!writable_)return ; writable_ = false; Update(); }
	

	void HandleReadEvent()  { if (read_callback_  != NULL) read_callback_();  }
	void HandleWriteEvent() { if (write_callback_ != NULL) write_callback_(); }
	void HandleCloseEvent() { if (close_callback_ != NULL) close_callback_(); }
	void HandleErrorEvent() { if (error_callback_ != NULL) error_callback_(); }

	void Reset();
	void Reset(EventLoop* event_loop, int fd = -1);
	void Remove();
	int Fd() const { return fd_; }
	void SetStatus(HandleStatus status) { status_ = status; }
	HandleStatus GetStatus() const { return status_; }

	void SetEventLoop(EventLoop* event_loop) { event_loop_ = event_loop; }
	
private:
	void Update();

	EventLoop* event_loop_;
	int fd_;
	HandleStatus status_;
	bool readable_;
	bool writable_;

	EventCallback read_callback_;
	EventCallback write_callback_;
	EventCallback close_callback_;
	EventCallback error_callback_;

};

} // namespace marz

#endif // __HANDLE_H__
