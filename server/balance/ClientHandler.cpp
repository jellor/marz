#include <json/json.h>
#include "ClientHandler.h"
#include "SvrMgr.h"
#include "Singleton.h"

namespace marz {

ClientHandler::ClientHandler(const ChannelPtr& channel_ptr):
Handler(channel_ptr),
http_context_()
{
    http_context_.SetHttpCallback(std::bind(&ClientHandler::OnHttp, this, std::placeholders::_1));
}

ClientHandler::~ClientHandler() {
}

void ClientHandler::OnActive(const ChannelPtr& channel_ptr) {

}

void ClientHandler::OnInactive(const ChannelPtr& channel_ptr) {

}

void ClientHandler::OnReceive(const ChannelPtr& channel_ptr)  {
    if (http_context_.Decode(channel_ptr) == false) {
        Close();
    }
}

void ClientHandler::OnHttp(const ChannelPtr& channel_ptr) 
{
    DLOG << http_context_.GetRequest().ToString();
   
    const HttpRequest request = http_context_.GetRequest();
    HttpResponse response(request.GetVersion());

    std::string uri = request.GetUri();

    SvrMgr& svr_mgr = Singleton<SvrMgr>::GetInstance();

    
    if (svr_mgr.GetServerCnt() <= 0) {
        Json::Value value;
        value["code"] = 1;
        value["msg"] = "No Valid Server";

        response.SetBody(value.toStyledString());
        DLOG << response.ToString();
        SendResponse(channel_ptr, response);
        return;
    }

    Json::Value value;

    const server_t* server = svr_mgr.GetServer();
    if (server == NULL) {   
        value["code"] = 2;
        value["msg"] = "Server Is Empty";
    } else {
        value["code"] = 0;
        value["msg"] = "Ok";

        value["priorIp"] = server->ip_addr_1;
        value["backupIp"] = server->ip_addr_2;
        value["port"] = server->port;
    }

    response.SetBody(value.toStyledString());
    DLOG << response.ToString();
    SendResponse(channel_ptr, response);
}

void ClientHandler::SendResponse(const ChannelPtr& channel_ptr, const HttpResponse& response) {
    RingBuffer* buffer = http_context_.Encode(response);
    Send(channel_ptr, buffer);
}

} // namespace marz
