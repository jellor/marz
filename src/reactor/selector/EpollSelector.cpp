#ifdef __linux__

#include <unistd.h>
#include "EpollSelector.h"
#include "Logger.h"

namespace marz {

EpollSelector::EpollSelector():
epfd_(epoll_create(1024)),
handle_set_(),
active_event_list_(64)
{

}

EpollSelector::~EpollSelector() {
	close(epfd_);
}

void EpollSelector::AddHandle(Handle* handle) {
	struct epoll_event ev;
	ev.data.ptr = handle;
	ev.events   = EPOLLPRI | EPOLLRDHUP;

	if (handle->IsWritable()) {
		ev.events |= EPOLLOUT;
	}
	if (handle->IsReadable()) {
		ev.events |= EPOLLIN;
	}
	if (epoll_ctl(epfd_, EPOLL_CTL_ADD, handle->Fd(), &ev) != 0) {
		WARN << "Cannot Add Event To Epoll";
		WARN << "errno => " << errno;
	}

	handle_set_.insert(handle);
	handle->SetStatus(ADD);
	DLOG << "AddHandler";
}

void EpollSelector::UpdateHandle(Handle* handle) {
	struct epoll_event ev;
	ev.data.ptr = handle;
	ev.events   = EPOLLPRI | EPOLLRDHUP;

	if (handle->IsWritable()) {
		ev.events |= EPOLLOUT;
	}
	if (handle->IsReadable()) {
		ev.events |= EPOLLIN;
	}
	if (epoll_ctl(epfd_, EPOLL_CTL_MOD, handle->Fd(), &ev) != 0) {
		WARN << "Cannot Mod Event To Epoll";
		WARN << "errno => " << errno;
	}

	DLOG << "UpdateHandler";
}

void EpollSelector::RemoveHandle(Handle* handle) {
	if (epoll_ctl(epfd_, EPOLL_CTL_DEL, handle->Fd(), NULL) != 0) {
		WARN << "Cannot DEL Event To Epoll";
		WARN << "errno => " << errno;
	}

	handle_set_.erase(handle);
	handle->SetStatus(DEL);
	DLOG << "RemoveHandler";
}

void EpollSelector::Dispatch(int64_t timeout_usec) {
	int active_count = 0;

	//active_event_list_.clear();

	if (timeout_usec <= 0) {
		active_count = epoll_wait(epfd_, &*active_event_list_.begin(), active_event_list_.size(), -1);
	} else {
		active_count = epoll_wait(epfd_, &*active_event_list_.begin(), active_event_list_.size(), timeout_usec / 1000);
	}

	if (active_count < 0) {
		WARN << "Epoll Wait Failed";
		WARN << "errno => " << errno;
		WARN << "errno => " << Loggr::GetError();

		if (errno != EINTR) {
		}
		return;
	} else {
		for (int i = 0; i < active_count; i ++) {
			Handle* active_handle = (Handle*) active_event_list_[i].data.ptr;
			struct epoll_event& active_event = active_event_list_[i];
			if (active_event.events & EPOLLERR) {
				DLOG << "EPOLLERR";
				active_handle->HandleErrorEvent();
			}
			if (active_event.events & EPOLLHUP) {
				DLOG << "EPOLLHUP";
			}
			if (active_event.events & EPOLLRDHUP) {
				DLOG << "EPOLLRDHUP";
				active_handle->HandleCloseEvent();
				active_handle->HandleReadEvent();
			}
			if (active_event.events & EPOLLPRI) {
				DLOG << "EPOLLPRI";
			}
			if (active_event.events & EPOLLIN) {
				DLOG << "EPOLLIN";
				active_handle->HandleReadEvent();
			}
			if (active_event.events & EPOLLOUT) {
				DLOG << "EPOLLOUT";
				active_handle->HandleWriteEvent();
			}
		}

		if (active_count == active_event_list_.size()) {
			active_event_list_.resize(active_event_list_.size() * 2);
		}
	}
	
}

} // namespace marz 

#endif // __linux__
