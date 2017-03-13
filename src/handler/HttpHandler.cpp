#include "HttpHandler.h"

namespace marz {

HttpHandler::HttpHandler(const ChannelPtr& channel_ptr):
Handler(channel_ptr),
http_context_()
{
    // 不要在构造函数中暴露this.
    // http_context_.SetHttpCallback(std::bind(&HttpHandler::OnHttp, this, std::placeholders::_1));
}

HttpHandler::~HttpHandler() {

}

void HttpHandler::OnReceive(const ChannelPtr& channel_ptr) {
    if (http_context_.Decode(channel_ptr) == false) {
        channel_ptr->Close();
    }
}

void HttpHandler::OnHttp(const ChannelPtr& channel_ptr) {
    DLOG << http_context_.GetRequest().ToString();
    std::string str("<html><head><title>xxx</title></head><body></body></html>");
    const HttpRequest request = http_context_.GetRequest();
    HttpResponse response(request.GetVersion());
    response.SetBody(str);
    DLOG << response.ToString();
    SendResponse(channel_ptr, response);
}

void HttpHandler::SendResponse(const ChannelPtr& channel_ptr, const HttpResponse& response) {
    RingBuffer* buffer = http_context_.Encode(response);
    channel_ptr->Send(buffer);
}

} // namespace marz
