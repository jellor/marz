#include <sstream>
#include "HttpRequest.h"

namespace marz {

const std::string HttpRequest::HTTP_GET 	= "GET";
const std::string HttpRequest::HTTP_PUT		= "PUT";
const std::string HttpRequest::HTTP_POST	= "POST";
const std::string HttpRequest::HTTP_HEAD	= "HEAD";
const std::string HttpRequest::HTTP_PATCH	= "PATCH";
const std::string HttpRequest::HTTP_TRACE	= "TRACE";
const std::string HttpRequest::HTTP_DELETE	= "DELETE";
const std::string HttpRequest::HTTP_OPTIONS	= "OPTIONS";
const std::string HttpRequest::HTTP_CONNECT	= "CONNECT";

HttpRequest::HttpRequest():
method_(HTTP_GET),
uri_("/"),
version_()
{

}

HttpRequest::HttpRequest(const std::string& method):
method_(method),
uri_("/"),
version_()
{

}

HttpRequest::HttpRequest(const std::string& method, const std::string& uri):
method_(method),
uri_(uri),
version_()
{

}

HttpRequest::HttpRequest(const std::string& method, const std::string& uri, const std::string& version):
method_(method),
uri_(uri),
version_(version)
{

}

HttpRequest::~HttpRequest() {

}

void HttpRequest::SetMethod(const std::string& method) {
	method_ = method;
}

const std::string& HttpRequest::GetMethod() const {
	return method_;
}

void HttpRequest::SetUri(const std::string& uri) {
	uri_ = uri;
}

const std::string& HttpRequest::GetUri() const {
	return uri_;
}

void HttpRequest::SetVersion(const std::string& version) {
	version_ = version;
}

const std::string& HttpRequest::GetVersion() const {
	return version_;
}

void HttpRequest::SetHeader(const std::string& name, const std::string& value) {
	headers_[name] = value;
}

const std::string& HttpRequest::GetHeader(const std::string& name) const {
	Header::const_iterator it = headers_.find(name);
	if (it != headers_.end()) {
		return (*it).second;
	} else {
		return "";
	}
}

void HttpRequest::SetBody(const std::string& body) {
	body_ = body;
}

const std::string& HttpRequest::GetBody() const {
	return body_;
}

const std::string HttpRequest::ToString() const {
    std::stringstream ss("");
    ss << std::endl;
    ss << GetMethod() << " " << GetUri() << " " << GetVersion() << std::endl;
    for (Header::const_iterator it = headers_.begin(); it != headers_.end(); it ++) {
        ss << (*it).first << ": " << (*it).second << std::endl;
    }
    if (! body_.empty()) {
        ss << "Request Body => " << body_ << std::endl;
    }
    return ss.str();
}

} // namespace marz
