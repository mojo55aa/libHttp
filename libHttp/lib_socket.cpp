#include "lib_socket.h"
#include <iostream>

using std::string;

// default recv buffer size 4KB (one page)
#define DEFAULT_BUFFER_SIZE 4096

// global SOCKET ERROR MSG
string g_sockErrMsg;

string GetSockErrMsg()
{
	return g_sockErrMsg;
}


LIBSocket::LIBSocket()
{
	g_sockErrMsg = "no error.";
#ifndef __LINUX__
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		mSocket = SOCKET_ERROR;
		throw "Win WSAStartup error.";
	}
#endif // !__LINUX__
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == mSocket)
	{
		mSocket = SOCKET_ERROR;
		throw "INVALID SOCKET.";
	}
}


LIBSocket::~LIBSocket()
{
#ifndef __LINUX__
	if (mSocket != SOCKET_ERROR)
	{
		closesocket(mSocket);
	}
	WSACleanup();
#else
	if (mSocket != SOCKET_ERROR)
	{
		close(mSocket);
	}
#endif // !__LINUX__
}


bool LIBSocket::Connect(const char* ip, unsigned int port)
{
	// IPv4
	struct sockaddr_in  sa = { 0 };
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);
	if (mSocket == SOCKET_ERROR)
	{
		g_sockErrMsg = "socket value is SOCKET_ERROR.";
		return false;
	}
	if (SOCKET_ERROR == connect(mSocket, (struct sockaddr*)&sa, sizeof(sa)))
	{
		g_sockErrMsg = "socket connect error.";
		return false;
	}
	return true;
}


__int64 LIBSocket::Send(const char* buff, unsigned int nbytes)
{
	if (mSocket == SOCKET_ERROR || buff == NULL)
	{
		g_sockErrMsg = "socket value is SOCKET_ERROR or buff is nullptr.";
		return SOCKET_ERROR;
	}
	return send(mSocket, buff, nbytes, 0);
}


__int64 LIBSocket::Recv(char* buff, unsigned int nbytes)
{
	if (mSocket == SOCKET_ERROR || buff == NULL)
	{
		g_sockErrMsg = "socket value is SOCKET_ERROR or buff is nullptr.";
		return SOCKET_ERROR;
	}
	memset(buff, 0, nbytes);
	// record how many bytes received
	unsigned int nread = 0;
	char* tmp = (char*)malloc(DEFAULT_BUFFER_SIZE);
	// malloc error, return.
	if (tmp == NULL)
	{
		g_sockErrMsg = "memory malloc fail.";
		return -1;
	}
	for (;;)
	{
		int n = 0;
		memset(tmp, 0, DEFAULT_BUFFER_SIZE);
		n = recv(mSocket, tmp, DEFAULT_BUFFER_SIZE, 0);
		//cout << "n = " << n << endl;
		if (n == -1)
		{
			g_sockErrMsg = "recv return -1.";
			// free memory
			free(tmp);
			return SOCKET_ERROR;
		}
		else if (n == 0)
		{
			free(tmp);
			return nread;
		}
		/*
		else if (n < DEFAULT_BUFFER_SIZE)
		{// recv done
			if (memcpy_s(buff + nread, nbytes - nread, tmp, n))
			{
				g_sockErrMsg = "buff does not have enough menory.";
				free(tmp);
				return -1;
			}
			free(tmp);
			return (nread + n);
		}*/
		// other, continue recv
		else
		{
			if (nbytes - nread < (unsigned int)n)
			{
				g_sockErrMsg = "buff does not have enough menory.";
				free(tmp);
				return -1;
			}
			memcpy(buff + nread, tmp, n);
			nread += n;
		}
	}
}

/**
 * @brief 域名解析出IP地址
 * @return 解析成功返回string ip地址，失败返回DNS_ERROR
 */
string GetIpByDomain(const char* domain)
{
	WSADATA wsadata;
	hostent *phostent;
	struct sockaddr_in sa;
	if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
	{
		throw("domain name resolution error, WSAStartup failed.");
	}

	phostent = gethostbyname(domain);
	if (phostent == NULL)
	{
		WSACleanup();
		return DNS_ERROR;
	}
	for (int n = 0; phostent->h_addr_list[n]; n++)
	{
		memcpy(&sa.sin_addr.s_addr, phostent->h_addr_list[n], phostent->h_length);
		WSACleanup();
		return inet_ntoa(sa.sin_addr);
	}
	return DNS_ERROR;
	WSACleanup();
}