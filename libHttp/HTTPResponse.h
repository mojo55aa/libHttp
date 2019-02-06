#pragma once
#include <string>
#include <map>
#include <vector>
#include "CookieJar.h"


class HTTPResponse
{
public:
	HTTPResponse(const char* res_buff, unsigned int nsize);
	virtual ~HTTPResponse();
	void SaveBinary(const char* filename);
	void SaveFile(const char* filename);

protected:
	std::string _get(const std::string key) const;
	void _parseHeader(const std::string& header);
	void _parseBody(const char* body);
	void _parseResponse(const char* response);
	bool _IsText() const;
	std::string _buffReadLine(const char* buff) const;

public:
	std::map<std::string, std::string> headers;		// 解析完成的headers
	int status_code;								// http状态码
	bool ok;										// 状态码是200
	std::string reason;								// 状态描述
	std::string text;								// 文本类型响应体
	char* content;									// bytes类型响应体
	std::string protocol;							// 报文协议及版本
	int content_size;								// 消息体长度

	CookieJar *cookies;								// cookies信息
private:
	void __split(const std::string& s, std::vector<std::string>& v, const std::string &sep) const;
	bool __key_equal(const std::string &a, const std::string &b) const;

protected:
	std::vector<std::string> _text_type;

private:
	const char* __buff_base;						// CPPRequests申请的buff地址
	unsigned int __buff_size;						// CPPRequests申请的buff size
};

