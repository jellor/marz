#ifndef __HTTP_CONTEXT_H__
#define __HTTP_CONTEXT_H__

#include <string>
#include <functional>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "RingBuffer.h"
#include "Channel.h"

namespace marz {

typedef enum {
	PARSE_LINE_OK    		= 0,
	PARSE_LINE_CONTINUE		= 1,
	PARSE_LINE_BAD 			= 2,
} PARSE_LINE_STATUS;

typedef enum {
	BAD = 0,
	OK  = 1,
} HTTP_CODE;

typedef enum {
	PARSE_REQUEST_LINE   = 0,
	PARSE_REQUEST_HEADER = 1,
	PARSE_REQUEST_BODY   = 2,
} PARSE_STATUS;

class HttpContext {

    typedef std::function<void(const ChannelPtr&)> HttpCallback;

public:
	HttpContext();
	~HttpContext();

	void SetHttpCallback(const HttpCallback& cb) { http_cb_ = cb; }

	RingBuffer* Encode(const HttpResponse& response);
	bool Decode(const ChannelPtr& channel_ptr);

	const HttpRequest& GetRequest() const { return request_; }


	PARSE_LINE_STATUS ParseLine(char* byte_buffer, int& start_index, const int end_index);
	bool ParseRequestLine(char* line);
	bool ParseRequestHeader(char* line);
	bool ParseRequestBody(const char* byte_buffer, int& start, const int end);
	int  ParseRequest(const ChannelPtr& channel_ptr, char* byte_buffer, const int n);

private:
	HttpRequest  request_;
	HttpResponse response_;
	PARSE_STATUS parse_status_;

	HttpCallback http_cb_;
};

} // namespace marz

#endif // __HTTP_CONTEXT_H__
