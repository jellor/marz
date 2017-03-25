#ifndef __IP_ADDRESS_H__
#define __IP_ADDRESS_H__

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Logger.h"

namespace marz {

class IpAddress {
public:
	IpAddress(): IpAddress(0) {}
	IpAddress(const std::string& ip, uint16_t port);
	IpAddress(uint16_t port, bool isLoopback = true);
	IpAddress(const struct sockaddr_in& addr): addr_(addr) {};

	~IpAddress() { 
		// DEBUG << "IpAddress Destructor ..."; 
	}

	void Reset();
	uint16_t Port() const;
	uint32_t Ip() const;
	std::string ToString() const;
	void SetAddr(const struct sockaddr_in& addr) { addr_ = addr; }
	const struct sockaddr_in& GetAddr() const { return addr_; }
	bool operator ==(const IpAddress& rhs) const;

	static std::string ToString(uint32_t ip, uint16_t port);

private:
	struct sockaddr_in addr_;

};

} // namespace marz

#endif // __IP_ADDRESS_H__
