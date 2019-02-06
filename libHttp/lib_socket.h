//#pragma once
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define DNS_ERROR "dns_err"


#ifdef __LINUX__
	#include <sys/socket.h>
	#define INVALID_SOCKET -1
	typedef int SOCKET;

#else
	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")

#endif // __LINUX__

class LIBSocket
{
public:
	LIBSocket();
	virtual ~LIBSocket();
	bool Connect(const char* ip, unsigned int port);
	__int64 Send(const char* buff, unsigned int nbytes);
	__int64 Recv(char* buff, unsigned int nbytes);
protected:
private:
	SOCKET mSocket;
};

std::string GetSockErrMsg();
std::string GetIpByDomain(const char* domain);