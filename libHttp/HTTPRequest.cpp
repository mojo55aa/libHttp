#include "HTTPRequest.h"
#include <string>
#include <algorithm>

using std::string;
using std::map;



HTTPRequest::HTTPRequest()
{
	memset(this->m_ip, 0, 128);
	this->m_port = 0;
	this->m_method = "GET";
	this->m_http = "HTTP";
	this->__m_reqBody = "";
	this->__m_req = "";
	this->m_sourceUrl = "";
	// set default User-Agent
	this->AddHeaderItem("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.80 Safari/537.36");
	this->AddHeaderItem("Connection", "close");
}


HTTPRequest::~HTTPRequest()
{
}

/**
 * @brief ��map headers�����ֵ
 */
void HTTPRequest::AddHeaderItem(string key, string value)
{
	if (!__IsKeyExist(key))
	{
		this->__m_reqHeaders.insert(std::pair<string, string>(key, value));
		return;
	}
	std::pair<string, string>* tmp;
	tmp = __FindKey(key);
	if (!tmp)
	{
		this->__m_reqHeaders.insert(std::pair<string, string>(key, value));
	}
	else
	{
		// ɾ���Ѿ����ڵģ��������
		this->__m_reqHeaders.erase(tmp->first);
		this->__m_reqHeaders[key] = value;
		delete tmp;
	}
}

 void HTTPRequest::AddHeaderItem(std::map<std::string, std::string> dict)
 {
	 map<string, string>::iterator iter;
	 for (iter = dict.begin(); iter != dict.end(); iter++)
	 {
		 this->AddHeaderItem(iter->first, iter->second);
	 }
 }

/**
 * @brief ����reqBody
 */
void HTTPRequest::SetRequestData(std::string req)
{
	this->__m_reqBody = req;
}

/**
 * @brief ����reqBody
 */
void HTTPRequest::SetRequestData(map<std::string, string> reqdict)
{
	map<string, string>::iterator iter;
	this->__m_reqBody = "";
	for (iter = reqdict.begin(); iter != reqdict.end(); iter++)
	{
		this->__m_reqBody += "&";
		this->__m_reqBody += iter->first + "=" + iter->second;
	}
	this->__m_reqBody.erase(0, 1);
}

/**
 * @brief ���key��map reqHeaders���Ƿ���ڣ����Դ�Сд�Ƚ�
 */
bool HTTPRequest::__IsKeyExist(const string key) const
{
	map<string, string>::const_iterator iter;
	for (iter = __m_reqHeaders.begin(); iter != this->__m_reqHeaders.end(); iter++)
	{
		string _a, _b;
		_a.resize(iter->first.size());
		_b.resize(key.size());
		std::transform(iter->first.begin(), iter->first.end(), _a.begin(), ::tolower);
		std::transform(key.begin(), key.end(), _b.begin(), ::tolower);
		if (_a == _b)
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief ����keyֵ�ҵ�map reqHeaders�е�ֵ
 * @return �ҵ��˷��طǿ�pairָ�룬û�ҵ�����NULL
 */
std::pair<string, string>* HTTPRequest::__FindKey(const std::string key) const
{
	map<string, string>::const_iterator iter;
	iter = this->__m_reqHeaders.begin();
	while (iter != this->__m_reqHeaders.end())
	{
		string _a, _b;
		_a.resize(iter->first.size());
		_b.resize(key.size());
		std::transform(iter->first.begin(), iter->first.end(), _a.begin(), ::tolower);
		std::transform(key.begin(), key.end(), _b.begin(), ::tolower);
		if (_a == _b)
		{
			std::pair<string, string> *tmp = new std::pair<string, string>;
			*tmp = std::make_pair(iter->first, iter->second);
			return tmp;
		}
		iter++;
	}
	return NULL;
}

/**
 * @brief �������ݣ�����HTTP Request
 */
void HTTPRequest::Parse()
{
	string requestUrl = this->m_sourceUrl;
	if (this->m_method == "GET")
	{
		if (!this->__m_reqBody.empty())
		{
			requestUrl += "?" + this->__m_reqBody;
		}
	}
	this->__m_req = this->m_method + " " + requestUrl + " HTTP/1.1\r\n";
	map<string, string>::iterator iter;
	for (iter = this->__m_reqHeaders.begin(); iter != this->__m_reqHeaders.end(); iter++)
	{
		this->__m_req += iter->first + ": ";
		this->__m_req += iter->second;
		this->__m_req += "\r\n";
	}
	this->__m_req += "\r\n";
	// �����POST�������������
	if (this->m_method == "POST")
	{
		this->__m_req += this->__m_reqBody;
		this->__m_req += "\r\n\r\n";
	}
}

/**
 * @brief ������װ�õ�http�����ַ���
 */
string HTTPRequest::GetRequestString()
{
	if (this->__m_req == "")
	{
		this->Parse();
	}
	return this->__m_req;
}