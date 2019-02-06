#pragma once
#include "CPPRequests.h"
#include "CookieJar.h"

class HTTPSession :
	public CPPRequests
{
public:
	HTTPSession();
	virtual ~HTTPSession();

	HTTPResponse* Get(const char* url, std::map<std::string, std::string>* data=NULL, std::map<std::string, std::string>* headers=NULL);
	HTTPResponse* Get(const char* url, std::string data="", std::map<std::string, std::string>* headers=NULL);
	HTTPResponse* Post(const char* url, std::string data="", std::map<std::string, std::string>* headers = NULL);
	HTTPResponse* Post(const char* url, std::map<std::string, std::string>* data = NULL, std::map<std::string, std::string>* headers = NULL);

private:
	void __cookie_process();
	void __request_prepare(std::map<std::string, std::string>& headers);

private:
	CookieJar* cookies;
};

