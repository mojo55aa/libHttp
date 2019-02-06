#include "HTTPSession.h"
using namespace std;



HTTPSession::HTTPSession()
{
	//TODO CookieJar拷贝构造
	this->cookies = new CookieJar();
}


HTTPSession::~HTTPSession()
{
	delete this->cookies;
}

HTTPResponse * HTTPSession::Get(const char * url, std::map<std::string, std::string>* data, std::map<std::string, std::string>* headers)
{
	this->__request_prepare(*headers);
	CPPRequests::Get(url, data, headers);
	this->__cookie_process();
	return this->httpResponse;
}

HTTPResponse * HTTPSession::Get(const char * url, std::string data, std::map<std::string, std::string>* headers)
{
	this->__request_prepare(*headers);
	CPPRequests::Get(url, data, headers);
	this->__cookie_process();
	return this->httpResponse;
}

HTTPResponse * HTTPSession::Post(const char * url, std::string data, std::map<std::string, std::string>* headers)
{
	this->__request_prepare(*headers);
	CPPRequests::Post(url, data, headers);
	this->__cookie_process();
	return this->httpResponse;
}

HTTPResponse * HTTPSession::Post(const char * url, std::map<std::string, std::string>* data, std::map<std::string, std::string>* headers)
{
	this->__request_prepare(*headers);
	CPPRequests::Post(url, data, headers);
	this->__cookie_process();
	return this->httpResponse;
}

void HTTPSession::__cookie_process()
{
	if (nullptr == this->httpResponse)
	{
		return;
	}
	// 更新cookies
	vector<CookieUnit> cookie_list = this->httpResponse->cookies->get_cookies();
	for (auto c : cookie_list)
	{
		this->cookies->set_cookie(&c);
	}
}

void HTTPSession::__request_prepare(map<string, string>& headers)
{
	string cookie = this->cookies->cookies_for_server();
	if ("" == cookie)
	{
		return;
	}

	if (NULL == &headers)
	{
		map<string, string> h;
		headers = h;
	}
	//TODO headers已经存在cookie如何处理
	headers["Cookie"] = cookie;
}
