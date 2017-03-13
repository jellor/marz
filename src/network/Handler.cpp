#include "Handler.h"
#include "Logger.h"

namespace marz {

Handler::Handler(const ChannelPtr& channel_ptr):
channel_ptr_(channel_ptr)
{
	DLOG << "Handler Constructor ...";
}

Handler::~Handler() {
	DLOG << "Handler Destructor ...";
}

/*---------------------------------  Up Stream  ----------------------------------*/

void Handler::OnActive(const ChannelPtr& channel_ptr) {
	DLOG << "Fire Active";
}

void Handler::OnReceive(const ChannelPtr& channel_ptr) {
	DLOG << "Fire Receive";
}

void Handler::OnInactive(const ChannelPtr& channel_ptr) {
	DLOG << "Fire Inactive";
}

void Handler::OnError(const ChannelPtr& channel_ptr) {
	DLOG << "Fire Error";
}

/*---------------------------------- Down Stream ----------------------------------*/

void Handler::Send(const ChannelPtr& channel_ptr, const char* data, int len) {
	DLOG << "Send";
	channel_ptr->Send(data, len);
}

void Handler::Send(const ChannelPtr& channel_ptr, const RingBuffer* buffer) {
	DLOG << "Send";
	channel_ptr->Send(buffer);
}

void Handler::ShutdownReceive(const ChannelPtr& channel_ptr) {
	DLOG << "Shutdown Receive";
	channel_ptr->ShutdownReceive();
}

void Handler::ShutdownSend(const ChannelPtr& channel_ptr) {
	DLOG << "Shutdown Send";
	channel_ptr->ShutdownSend();
}

void Handler::Close(const ChannelPtr& channel_ptr) {
	DLOG << "Close Channel";
	channel_ptr->Close();
}

} // namespace marz
