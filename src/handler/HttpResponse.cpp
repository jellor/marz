#include <sstream>
#include <stdlib.h>
#include "HttpResponse.h"

namespace marz {

HttpResponse::HttpResponse():
version_("HTTP/1.0"),
status_code_(200),
status_message_("OK"),
headers_()
{

}

HttpResponse::HttpResponse(const std::string& version):
version_(version),
status_code_(200),
status_message_("OK"),
headers_()
{

}

HttpResponse::~HttpResponse() {

}

void HttpResponse::SetVersion(const std::string& version) {
	version_ = version;
}

const std::string& HttpResponse::GetVersion() const {
	return version_;
}

void HttpResponse::SetStatusCode(int status_code) {
	status_code_ = status_code;
}

int HttpResponse::GetStatusCode() const {
	return status_code_;
}

void HttpResponse::SetStatusMessage(const std::string& status_message) {
	status_message_ = status_message;
}

const std::string& HttpResponse::GetStatusMessage() const {
	return status_message_;
}

void HttpResponse::SetHeader(const std::string& name, const std::string& value) {
	headers_[name] = value;
}

const std::string& HttpResponse::GetHeader(const std::string& name) const {
	Header::const_iterator it = headers_.find(name);
	if (it != headers_.end()) {
		return (*it).second;
	} else {
		return "";
	}
}

void HttpResponse::SetBody(const std::string& body) {
    std::stringstream ss("");
    ss << body.size();
    SetHeader("Content-Length", ss.str());
	body_ = body;
}

void HttpResponse::SetBody(const char* data, int length) {
    std::stringstream ss("");
    ss << length;
    SetHeader("Content-Length", ss.str());
    body_ = data;
}

const std::string& HttpResponse::GetBody() const {
	return body_;
}

const std::string HttpResponse::ToString() const {
    std::stringstream ss("");

    ss << GetVersion() << " " << GetStatusCode() << " " << GetStatusMessage() << "\r\n";
    for (Header::const_iterator it = headers_.begin(); it != headers_.end(); it ++) {
        ss << (*it).first << ": " << (*it).second << "\r\n";
    }
    ss << "\r\n";
    if (! body_.empty()) {
        ss << GetBody();
    }
    return ss.str();
}

} // namespace marz
