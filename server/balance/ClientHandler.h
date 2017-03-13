#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include "HttpContext.h"
#include "Handler.h"

namespace marz {

class ClientHandler : public Handler {
public:
	ClientHandler(const ChannelPtr& channel_ptr);
	virtual ~ClientHandler();

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnReceive(const ChannelPtr& channel_ptr) override;

	virtual void OnHttp(const ChannelPtr& channel_ptr);

	void SendResponse(const ChannelPtr& channel_ptr, const HttpResponse& response);

private:
	HttpContext http_context_;

};

} // namespace marz

#endif // __CLIENT_HANDLER_H__
