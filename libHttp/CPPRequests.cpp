#define _CRT_SECURE_NO_WARNINGS
#include "CPPRequests.h"
#include "lib_socket.h"
#include <iostream>
#include <fstream>
#include "mojo_tools.h"
#include <typeinfo>
using namespace std;

#define DEFAULT_BUFF_SIZE 0x80000	//512KB


CPPRequests::CPPRequests()
{
	this->__m_buff_size = DEFAULT_BUFF_SIZE;
	this->__m_buff = nullptr;
	this->httpResponse = nullptr;
}


CPPRequests::~CPPRequests()
{
	// 释放Recv Buff
	if (this->__m_buff != nullptr)
	{
		delete[] this->__m_buff;
	}

	// 释放response
	if (this->httpResponse != nullptr)
	{
		delete this->httpResponse;
	}
}

HTTPResponse* CPPRequests::Get(const char * url, string data, map<string, string>* headers)
{
	// Request
	HTTPRequest httpRequest;
	this->_request_init(&httpRequest, url, HTTP_GET);
	// 处理data和headers
	if (NULL != headers)
	{
		httpRequest.AddHeaderItem(*headers);
	}
	if (!data.empty())
	{
		httpRequest.SetRequestData(data);
	}

	return this->_request_action(&httpRequest);
}

HTTPResponse* CPPRequests::Get(const char* url, map<string, string>* data, map<string, string>* headers)
{
	// Request
	HTTPRequest httpRequest;
	this->_request_init(&httpRequest, url, HTTP_GET);
	// 处理data和headers
	if (NULL != headers)
	{
		httpRequest.AddHeaderItem(*headers);
	}
	if (NULL != data)
	{
		httpRequest.SetRequestData(*data);
	}

	return this->_request_action(&httpRequest);
}

HTTPResponse* CPPRequests::Post(const char * url, string data, map<string, string>* headers)
{
	// Request
	HTTPRequest httpRequest;
	this->_request_init(&httpRequest, url, HTTP_POST);
	// 处理data和headers
	if (!data.empty())
	{
		httpRequest.SetRequestData(data);
	}
	if (NULL != headers)
	{
		httpRequest.AddHeaderItem(*headers);
	}

	return this->_request_action(&httpRequest);
}

HTTPResponse* CPPRequests::Post(const char * url, map<string, string>* data, map<string, string>* headers)
{
	// Request
	HTTPRequest httpRequest;
	this->_request_init(&httpRequest, url, HTTP_POST);
	// 处理data和headers
	if (NULL != data)
	{
		httpRequest.SetRequestData(*data);
	}
	if (NULL != headers)
	{
		httpRequest.AddHeaderItem(*headers);
	}
	
	return this->_request_action(&httpRequest);
}

void CPPRequests::Options(REQUESTS_OPTIONS opt, int val)
{
	switch (opt)
	{
	case BUFF_SIZE:
	{
		if (val < DEFAULT_BUFF_SIZE)
		{
			return;
		}
		this->__m_buff_size = val;
		break;
	}
	case TIME_OUT:
	{
		break;
	}
	default:
		break;
	}
}

/**
* @brief 解析url的method,domain,resource
*/
void CPPRequests::__ParseUrl(const string url, struct URL & urlParsed)
{
	// parse method
	string method = url.substr(0, 5);
	if ("https" == method)
	{
		urlParsed.http_type = "HTTPS";
	}
	else if ("http:" == method)
	{
		urlParsed.http_type = "HTTP";
	}
	else
	{
		throw("url parse error, not http or https.");
	}
	// parse domain
	urlParsed.port = 0;
	int pos;
	int head_length;
	head_length = urlParsed.http_type.length() + 3;
	pos = url.find("/", head_length);
	if (pos == string::npos)
	{
		urlParsed.domain = url.substr(head_length);
		urlParsed.resource_path = "/";
	}
	else
	{
		urlParsed.domain = url.substr(head_length, pos - head_length);
		// parse resource
		urlParsed.resource_path = url.substr(pos);
	}

	// url中有端口号
	if (urlParsed.domain.find(":") != string::npos)
	{
		vector<string> _t;
		mojo_tools::split(urlParsed.domain, _t, ":");
		urlParsed.domain = _t[0];
		urlParsed.port = stoi(_t[1]);
	}
}

/**
 * @brief request前的初始化，解析url，初始化HTTPRequest
 * @return 成功返回true，出错返回false
 */
bool CPPRequests::_request_init(HTTPRequest* request, const char* url, HTTP_METHOD method)
{
	struct URL url_parsed;
	this->__ParseUrl(url, url_parsed);
	request->m_http = url_parsed.http_type;
	request->m_sourceUrl = url_parsed.resource_path;
	if (method == HTTP_GET)
	{
		request->m_method = "GET";
	}
	else
	{
		request->m_method = "POST";
	}
	// dns
	string ip = GetIpByDomain(url_parsed.domain.c_str());
	if (ip == DNS_ERROR)
	{
		throw("DNS ERROR.");
		return false;
	}
	request->AddHeaderItem("Host", url_parsed.domain);
	strcpy(request->m_ip, ip.c_str());

	//TODO 先统一80端口，ssl时再补充
	if (url_parsed.port)
	{
		request->m_port = url_parsed.port;
		return true;
	}
	request->m_port = 80;
	return true;
}

/**
 * @brief socket发送请求的部分，处理Response
 * @httpRequest 请求的request
 * @return 返回HTTPResponse*指针 or nullptr
 */
HTTPResponse* CPPRequests::_request_action(HTTPRequest * httpRequest)
{
	// socket
	LIBSocket sock;
	bool bRes = sock.Connect(httpRequest->m_ip, httpRequest->m_port);
	if (!bRes)
	{
		return nullptr;
	}
	string send_data = httpRequest->GetRequestString();
	cout << send_data << endl;
	bRes = sock.Send(send_data.c_str(), send_data.size());
	this->__m_buff = new char[this->__m_buff_size]();
	cout << sock.Recv(this->__m_buff, this->__m_buff_size) << endl;
	cout << this->__m_buff << endl;

	// Response
	if (nullptr != this->httpResponse)
	{
		delete this->httpResponse;
	}
	this->httpResponse = new HTTPResponse(this->__m_buff, this->__m_buff_size);
	return httpResponse;
}
