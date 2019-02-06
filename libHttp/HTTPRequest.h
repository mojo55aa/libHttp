#pragma once
#include <map>
#include <string>


class HTTPRequest
{
public:
	HTTPRequest();
	virtual ~HTTPRequest();
	void AddHeaderItem(std::string key, std::string value);
	void AddHeaderItem(std::map<std::string, std::string> dict);
	void SetRequestData(std::string);
	void SetRequestData(std::map<std::string, std::string>);
	std::string GetRequestString();
	void Parse();

public:
	char m_ip[128];	/* server ip */
	unsigned int m_port;	/* server port */
	std::string m_method;	/* GET/POST */
	std::string m_http;	/* HTTP/HTTPS */
	std::string m_sourceUrl;	/* request url */

private:
	bool __IsKeyExist(const std::string key) const;
	std::pair<std::string, std::string>* __FindKey(const std::string key) const;

private:
	std::map<std::string, std::string> __m_reqHeaders;		/* HTTP Request Headers */
	std::string __m_reqBody;		/* HTTP Request Body */
	std::string __m_req;		/* HTTP Request (send str)*/
};
