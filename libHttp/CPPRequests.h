#pragma once
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <string>
#include <map>


struct URL 
{
	std::string http_type;
	std::string domain;
	std::string resource_path;
	int port;
};

typedef enum http_method
{
	HTTP_GET,
	HTTP_POST
}HTTP_METHOD;

typedef enum requests_options
{
	BUFF_SIZE,
	TIME_OUT
}REQUESTS_OPTIONS;

class CPPRequests
{
public:
	CPPRequests();
	virtual ~CPPRequests();

	virtual HTTPResponse* Get(const char* url, std::string data="", std::map<std::string, std::string>* headers=NULL);
	virtual HTTPResponse* Get(const char* url, std::map<std::string, std::string>* data=NULL, std::map<std::string, std::string>* headers=NULL);
	virtual HTTPResponse* Post(const char* url, std::string data="", std::map<std::string, std::string>* headers=NULL);
	virtual HTTPResponse* Post(const char* url, std::map<std::string, std::string>* data=NULL, std::map<std::string, std::string>* headers=NULL);
	
	void Options(REQUESTS_OPTIONS opt, int val);
private:
	void __ParseUrl(const std::string url, struct URL &urlParsed);
protected:
	bool _request_init(HTTPRequest* request, const char* url, HTTP_METHOD method);
	HTTPResponse* _request_action(HTTPRequest* httpRequest);

private:
	int __m_buff_size;	// socket返回的数据存放在这个缓冲区中
	char* __m_buff;		// 缓冲区大小

protected:
	HTTPResponse* httpResponse;
};

