#include "Handle.h"
#include "Logger.h"

namespace marz {

Handle::Handle(EventLoop* event_loop, int fd):
event_loop_(event_loop),
fd_(fd),
status_(NEW),
readable_(false),
writable_(false),
read_callback_(NULL),
write_callback_(NULL),
close_callback_(NULL),
error_callback_(NULL)
{

}

Handle::~Handle() {
	// DEBUG << "Handle Destructor ...";
	readable_ = false;
	writable_ = false;
	Remove();
}

void Handle::Reset() {
	Remove();
	event_loop_ 	= NULL;
	fd_         	= -1;
	status_     	= NEW;
	readable_   	= false;
	writable_  	 	= false;
	read_callback_  = NULL;
	write_callback_ = NULL;
	close_callback_ = NULL;
	error_callback_ = NULL;
}

void Handle::Reset(EventLoop* event_loop, int fd) {
	Remove();
	event_loop_ 	= event_loop;
	fd_         	= fd;
	status_     	= NEW;
	readable_   	= false;
	writable_  	 	= false;
	read_callback_  = NULL;
	write_callback_ = NULL;
	close_callback_ = NULL;
	error_callback_ = NULL;
}

void Handle::Remove() {
	if (status_ == ADD) {
		event_loop_->RemoveHandle(this);
	}

	readable_ = false;
	writable_ = false;
}

void Handle::Update() {
	if ((status_ == NEW || status_ == DEL) && (IsReadable() || IsWritable())) {
		event_loop_->AddHandle(this);
	} else if (status_ == ADD && (IsReadable() || IsWritable())) {
		event_loop_->UpdateHandle(this);
	} else {
		event_loop_->RemoveHandle(this);
	}
}

} // namespace marz
