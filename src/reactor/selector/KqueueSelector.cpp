#ifdef __APPLE__

#include <unistd.h>
#include "KqueueSelector.h"
#include "Logger.h"

namespace marz {

KqueueSelector::KqueueSelector():
kqfd_(kqueue()),
handle_set_(),
active_event_list_(64)
{

}

KqueueSelector::~KqueueSelector() {
	close(kqfd_);
}

void KqueueSelector::AddHandle(Handle* handle) {
	struct kevent event[2];
	if (handle->IsReadable()) {
		EV_SET(&event[0], handle->Fd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, handle);	
	} else {
		EV_SET(&event[0], handle->Fd(), EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, handle);
	}
	if (handle->IsWritable()) {
		EV_SET(&event[1], handle->Fd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, handle);
	} else {
		EV_SET(&event[1], handle->Fd(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, handle);
	}

	kevent(kqfd_, event, 2, NULL, 0, NULL);
	handle_set_.insert(handle);
	handle->SetStatus(ADD);
}

void KqueueSelector::UpdateHandle(Handle* handle) {
	struct kevent event[2];

	if (handle->IsReadable()) {
		EV_SET(&event[0], handle->Fd(), EVFILT_READ, EV_ENABLE, 0, 0, handle);	
	} else {
		EV_SET(&event[0], handle->Fd(), EVFILT_READ, EV_DISABLE, 0, 0, handle);
	}
	if (handle->IsWritable()) {
		EV_SET(&event[1], handle->Fd(), EVFILT_WRITE, EV_ENABLE, 0, 0, handle);
	} else {
		EV_SET(&event[1], handle->Fd(), EVFILT_WRITE, EV_DISABLE, 0, 0, handle);
	}

	kevent(kqfd_, event, 2, NULL, 0, NULL);
}

void KqueueSelector::RemoveHandle(Handle* handle) {
	struct kevent event[2];

	// todo: marge kevent(kqfd_, event, 2, NULL, 0, NULL)
	EV_SET(&event[0], handle->Fd(), EVFILT_READ, EV_DELETE, 0, 0, handle);
	kevent(kqfd_, &event[0], 1, NULL, 0, NULL);

	EV_SET(&event[1], handle->Fd(), EVFILT_WRITE, EV_DELETE, 0, 0, handle);
	kevent(kqfd_, &event[1], 1, NULL, 0, NULL);

	handle_set_.erase(handle);
	handle->SetStatus(DEL);
}

void KqueueSelector::Dispatch(int64_t timeout_usec) {
	int active_count = 0;

	// active_event_list_.clear();

	if (timeout_usec <= 0) {
		active_count = kevent(kqfd_, NULL, 0, &*active_event_list_.begin(), active_event_list_.size(), NULL);
	} else {
		struct timespec timeout;
		timeout.tv_sec  = timeout_usec / 1000000;
		timeout.tv_nsec = (timeout_usec % 1000000) * 1000;
		active_count = kevent(kqfd_, NULL, 0, &*active_event_list_.begin(), active_event_list_.size(), &timeout);
	}

	if (active_count < 0) {
		WLOG << "Epoll Wait Failed";
		WLOG << "errno => " << errno;
		WLOG << "errno => " << Logger::GetError();
	} else {
		for (int i = 0; i < active_count; i ++) {
			struct kevent& active_event = active_event_list_[i];
			Handle* active_handle = (Handle*) active_event.udata;

			if (active_event.flags & EV_ERROR) {
				DLOG << "EV_ERROR";
				active_handle->HandleErrorEvent();
			}
			if (active_event.flags & EV_EOF) {
				DLOG << "EV_EOF";
				//active_handle->handleCloseEvent();
			}
			if (active_event.filter == EVFILT_READ) {
				DLOG << "EVFILT_READ";
				active_handle->HandleReadEvent();
			}
			if (active_event.filter == EVFILT_WRITE) {
				DLOG << "EVFILT_WRITE";
				active_handle->HandleWriteEvent();
			}
		}

		if (active_count == active_event_list_.size()) {
			active_event_list_.resize(active_event_list_.size() * 2);
		}
	}
	
}

} // namespace marz

#endif // __APPLE__
