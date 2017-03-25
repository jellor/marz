#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "NonCopyable.h"
#include "Channel.h"

namespace marz {

class Handler : public NonCopyable {
public:
	Handler(const ChannelPtr& channel_ptr);
	virtual ~Handler();

	virtual void OnActive(const ChannelPtr& channel_ptr);
	virtual void OnReceive(const ChannelPtr& channel_ptr);
	virtual void OnInactive(const ChannelPtr& channel_ptr);
	virtual void OnError(const ChannelPtr& channel_ptr);

	virtual void Send(const char* data, int len);
	virtual void Send(const RingBuffer* buffer);
	virtual void ShutdownReceive();
	virtual void ShutdownSend();
	virtual void Close();

	virtual void Send(const ChannelPtr& channel_ptr, const char* data, int len);
	virtual void Send(const ChannelPtr& channel_ptr, const RingBuffer* buffer);
	virtual void ShutdownReceive(const ChannelPtr& channel_ptr);
	virtual void ShutdownSend(const ChannelPtr& channel_ptr);
	virtual void Close(const ChannelPtr& channel_ptr);
	
protected:
	ChannelPtr channel_ptr_;
	
};

} // namespace marz

#endif // __HANDLER_H__
