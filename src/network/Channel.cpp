#include <exception>
#include "Channel.h"
#include "Exception.h"
#include "Logger.h"
#include "Handler.h"

namespace marz {

Channel::Channel(EventLoop* event_loop, int sockfd, const IpAddress& local_address, const IpAddress& peer_address):
event_loop_(event_loop),
socket_(sockfd),
local_address_(local_address),
peer_address_(peer_address),
handle_(event_loop_, socket_.Fd()),
out_buffer_(),
in_buffer_(),
opened_(true),
read_callback_(NULL),
write_callback_(NULL),
close_callback_(NULL),
error_callback_(NULL),
handler_(NULL)
{
    socket_.SetNoDelay(true);
    handle_.SetReadCallback(std::bind(&Channel::HandleReadEvent, this));
    handle_.SetWriteCallback(std::bind(&Channel::HandleWriteEvent, this));
    handle_.EnableRead();
}

Channel::~Channel() {
    DLOG << "Channel Destructor ... ";
    socket_.Close();
}

void Channel::ShutdownReceive() {
    event_loop_->DoFunc(std::bind(&Channel::ShutdownReceiveInEventLoop, this));
}

void Channel::ShutdownReceiveInEventLoop() {
    DLOG << "Shutdown Receive In Event Loop";
    socket_.ShutdownReceive();
}

void Channel::ShutdownSend() {
    event_loop_->DoFunc(std::bind(&Channel::ShutdownSendInEventLoop, this));
}

void Channel::ShutdownSendInEventLoop() {
    DLOG << "Shutdown Send In Event Loop";
    socket_.ShutdownSend();
}

void Channel::Close() {
    event_loop_->DoFunc(std::bind(&Channel::CloseInEventLoop, this));
}

void Channel::CloseInEventLoop() {
    DLOG << "Close In Event Loop";
    opened_ = false;

    socket_.Close();
}

std::string Channel::ToString() const {
    return "";
}

void Channel::Send(const RingBuffer* buffer) {
    if (!opened_) {
        WLOG << "Channel Is Not Opened";
        return ;
    }
    event_loop_->DoFunc(std::bind(&Channel::SendBufferInEventLoop, this, buffer));
}

void Channel::Send(const char* data, unsigned int size) {
    if (!opened_) {
        WLOG << "Channel Is Not Opened";
        return ;
    }
    event_loop_->DoFunc(std::bind(&Channel::SendInEventLoop, this, data, size));
}

int Channel::Read(char* data, unsigned int size) {
    if (!opened_) {
        WLOG << "Channel Is Not Opened";
        return 0;
    }
    int ret = in_buffer_.ReadBytes(data, size);
    return ret;
}

int Channel::Write(const char* data, unsigned int size) {
    // maybe not in EventLoop.
    int ret = out_buffer_.Write(data, size);
    handle_.EnableWrite();
    return ret;
}

void Channel::SendBufferInEventLoop(const RingBuffer* buffer) {
    // DLOG << "Handler Writable " << handle_.IsWritable();
    // DLOG << "Handler Status " << handle_.GetStatus();
    // DLOG << "out_buffer size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();
    // DLOG << "RingBuffer Size " << buffer->Size() << " Capacity " << buffer->Capacity();
    if (out_buffer_.Empty()) {
        int written_size = ::write(socket_.Fd(), buffer->Head(), buffer->Size());

        if (written_size < 0) {
            written_size = 0;
            WLOG << "Syscall ::write() Occurs Errno => " << errno;
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {

            }
        }

        if (written_size < buffer->Size()) {
            out_buffer_.Write(buffer->Head() + written_size, buffer->Size() - written_size);
            handle_.EnableWrite();
        }

    } else {
        DLOG << "out_buffer size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();
        out_buffer_.Write(buffer->Head(), buffer->Size());
        handle_.EnableWrite();
    }

    delete buffer;
}

void Channel::SendInEventLoop(const char* data, unsigned int size) {

    // DLOG << "Handler Writable " << handle_.IsWritable();
    // DLOG << "Handler Status " << handle_.GetStatus();
    // DLOG << "out_buffer size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();

    if (out_buffer_.Empty()) {
        int written_size = ::write(socket_.Fd(), data, size);

        if (written_size < 0) {
            written_size = 0;
            WLOG << "Syscall ::write() Occurs Errno => " << errno;
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {

            }
        }

        if (written_size < size) {
            out_buffer_.Write(data + written_size, size - written_size);
            handle_.EnableWrite();
        }

    } else {
        DLOG << "out_buffer size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();
        out_buffer_.Write(data, size);
        handle_.EnableWrite();
    }

    delete data;
}

void Channel::HandleReadEvent() {
    DLOG << "in_buffer Size " << in_buffer_.Size() << " Capacity " << in_buffer_.Capacity();
    int size = in_buffer_.Read(socket_.Fd());

    if (size <= 0) {
        DLOG << "From Fd => " << socket_.Fd() << " Receive Size => " << size;
        DLOG << "errno => " << errno;
        DLOG << "errno => " << Logger::GetError();
        if (close_callback_ != NULL) {
            close_callback_(shared_from_this());
        }

        if (opened_ == true) {
            Close();
        }

    } else {
        DLOG << "in_buffer Size " << in_buffer_.Size() << " Capacity " << in_buffer_.Capacity();
        if (read_callback_ != NULL) {
            read_callback_(shared_from_this());
        }
    }

}

void Channel::HandleWriteEvent() {
    if (write_callback_ != NULL) {
        write_callback_(shared_from_this());
    }

    DLOG << "Buffer Size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();

    out_buffer_.Write(socket_.Fd());

    if (out_buffer_.Empty()) {
        DLOG << "out_buffer_ Size " << out_buffer_.Size() << " Capacity " << out_buffer_.Capacity();
        handle_.DisableWrite();
    }

}

} // namespace marz
