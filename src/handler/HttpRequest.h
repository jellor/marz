#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include <string>

namespace marz {

class HttpRequest {
public:
	HttpRequest();
	HttpRequest(const std::string& method);
	HttpRequest(const std::string& method, const std::string& uri);
	HttpRequest(const std::string& method, const std::string& uri, const std::string& version);
	~HttpRequest();

	void SetMethod(const std::string& method);
	const std::string& GetMethod() const;

	void SetUri(const std::string& uri);
	const std::string& GetUri() const;

	void SetVersion(const std::string& version);
	const std::string& GetVersion() const;

	void SetHeader(const std::string& name, const std::string& value);
	const std::string& GetHeader(const std::string& name) const;

	void SetBody(const std::string& body);
	const std::string& GetBody() const;

	const std::string ToString() const;

private:
	typedef std::map<const std::string, std::string> Header;

	std::string method_;
	std::string uri_;
	std::string version_;
	Header headers_;
	std::string body_;

	static const std::string HTTP_GET;
	static const std::string HTTP_PUT;
	static const std::string HTTP_POST;
	static const std::string HTTP_HEAD;
	static const std::string HTTP_PATCH;
	static const std::string HTTP_TRACE;
	static const std::string HTTP_DELETE;
	static const std::string HTTP_OPTIONS;
	static const std::string HTTP_CONNECT;
};

} // namespace marz

#endif // __HTTP_REQUEST_H__
