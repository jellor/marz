#ifndef __HTTP_HANDLER_H__
#define __HTTP_HANDLER_H__

#include "HttpContext.h"
#include "Handler.h"

namespace marz {

class HttpHandler : public Handler {
public:
	HttpHandler(const ChannelPtr& channel_ptr);
	virtual ~HttpHandler();

	virtual void OnActive(const ChannelPtr& channel_ptr) override {
		http_context_.SetHttpCallback(std::bind(&HttpHandler::OnHttp, this, std::placeholders::_1));
	}

	virtual void OnInactive(const ChannelPtr& channel_ptr) override {

	}

	virtual void OnReceive(const ChannelPtr& channel_ptr) override;

	virtual void OnHttp(const ChannelPtr& channel_ptr);

	void SendResponse(const ChannelPtr& channel_ptr, const HttpResponse& response);

private:

	HttpContext http_context_;
};

} // namespace marz

#endif // __HTTP_HANDLER_H__
