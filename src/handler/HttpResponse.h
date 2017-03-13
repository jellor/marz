#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <map>
#include <string>

namespace marz {

class HttpResponse {
public:
	HttpResponse();
	HttpResponse(const std::string& version);
	~HttpResponse();

    void SetVersion(const std::string& version);
	const std::string& GetVersion() const;
	void SetStatusCode(int status_code);
	int GetStatusCode() const;
	void SetStatusMessage(const std::string& status_message);
	const std::string& GetStatusMessage() const;
	void SetHeader(const std::string& name, const std::string& value);
	const std::string& GetHeader(const std::string& name) const;
	void SetBody(const std::string& body);
	void SetBody(const char* data, int length);
	const std::string& GetBody() const;

	const std::string ToString() const;

private:
	typedef std::map<const std::string, std::string> Header;
	std::string version_;
	int status_code_;
	std::string status_message_;
	std::string body_;

	Header headers_;

};

} // namespace marz

#endif // __HTTP_RESPONSE_H__
