#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <curl/curl.h>
#include "Types.h"

namespace marz {

class HttpClient {
public:
	HttpClient(void);
	~HttpClient(void);

	CURLcode Get(const std::string& url, std::string& response);
	CURLcode Post(const std::string& url, const std::string& params, std::string& response);
	CURLcode GetS(const std::string& url, std::string& response, const char *ca_path);
	CURLcode PostS(const std::string& url, const std::string& params, std::string& response, const char *ca_path);
	
	std::string UploadByteFile(const std::string& url, void *data, size_t size);
	bool DownloadByteFile(const std::string& url, audio_t* audio);
}; 

} // marz

#endif // __HTTP_CLIENT_H__
