#include <assert.h>
#include "HttpClient.h"
#include "Logger.h"

namespace marz {

static size_t OnWriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string *str = static_cast<std::string*>(stream);
	if (str == NULL || ptr == NULL) {
		return -1;
	}

	char *data = static_cast<char*>(ptr);
	str->append(data, size * nmemb);
	return nmemb;
}

static size_t OnWriteBytes(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t len = size * nmemb;
	audio_t *audio = (audio_t*) stream;
	if (audio->data_len + len <= audio->file_size + 4) {
		memcpy(audio->data + audio->data_len, ptr, len);
		audio->data_len += len;
	}
	return len;
}

static size_t OnWriteString(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t len = size * nmemb;
	std::string *str = static_cast<std::string*>(stream);
	str->append((char*)ptr, len);
	return len;
}

HttpClient::HttpClient(void)
{
	DLOG << "crul version => " << curl_version();
}
	
HttpClient::~HttpClient(void)
{
	DLOG << "http client destroy in curl";
}

CURLcode HttpClient::Get(const std::string& url, std::string& response)
{
	assert(!url.empty());

	CURLcode ret;
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

	ret = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return ret;
}

CURLcode HttpClient::Post(const std::string& url, const std::string& params, std::string& response)
{
	assert(!url.empty() && !params.empty());
	CURLcode ret;
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

	ret = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return ret;
}

CURLcode GetS(const std::string& url, std::string& response, const char *ca_path)
{
	assert(!url.empty());

	CURLcode ret;
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

	if (ca_path == NULL) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	} else {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca_path);
	}

	ret = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return ret;
}
	
CURLcode PostS(const std::string& url, const std::string& params, std::string& response, const char *ca_path)
{
	assert(!url.empty() && !params.empty());
	CURLcode ret;
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

	if (ca_path == NULL) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	} else {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, ca_path);
	}

	ret = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return ret;
}

std::string HttpClient::UploadByteFile(const std::string& url, void *data, size_t size)
{
	assert(!url.empty());

	std::string retstr;
	
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		WLOG << "curl_easy_init failed";
		return "";
	}

	struct curl_slist *header_list = NULL;
	header_list = curl_slist_append(header_list, "Content-Type: multipart/form-data; boundary=WebKitFormBoundary8riBH6S4ZsoT69so");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	std::string body = "--WebKitFormBoundary8riBH6S4ZsoT69so\r\nContent-Disposition: form-data; name=\"file\"; filename=\"1.audio\";\r\nContent-Type: image/jpg\r\n\r\n";
	body.append((char*)data, size);
	std::string str = "\r\n--WebKitFormBoundary8riBH6S4ZsoT69so--\r\n\r\n";
	body.append(str.c_str(), str.size());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&retstr);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (code != CURLE_OK) {
		DLOG << "curl_easy_perform failed code => " << code << " ret => " << retstr;
		return "";
	}

	return retstr;
}

bool HttpClient::DownloadByteFile(const std::string& url, audio_t* audio)
{
	assert(!url.empty());

	bool ret = true;
	CURLcode code;
	CURL *curl = curl_easy_init();
	if (curl == NULL) {
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteBytes);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)audio);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);

	code = curl_easy_perform(curl);

	int retcode = 0;
	code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	if (code != CURLE_OK || retcode != 200) {
		WLOG << "curl_easy_perform failed, ret => " << ret << " code => " << code;
	}

	double size = 0;
	code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
	if (size != audio->file_size) {
		WLOG << "download size (" << size << ") != file size (" << audio->file_size << ")";
		ret = false;
	}

	curl_easy_cleanup(curl);
	return ret;
}

} // namespace marz
