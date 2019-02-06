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
	std::map<std::string, std::string> headers;		// ������ɵ�headers
	int status_code;								// http״̬��
	bool ok;										// ״̬����200
	std::string reason;								// ״̬����
	std::string text;								// �ı�������Ӧ��
	char* content;									// bytes������Ӧ��
	std::string protocol;							// ����Э�鼰�汾
	int content_size;								// ��Ϣ�峤��

	CookieJar *cookies;								// cookies��Ϣ
private:
	void __split(const std::string& s, std::vector<std::string>& v, const std::string &sep) const;
	bool __key_equal(const std::string &a, const std::string &b) const;

protected:
	std::vector<std::string> _text_type;

private:
	const char* __buff_base;						// CPPRequests�����buff��ַ
	unsigned int __buff_size;						// CPPRequests�����buff size
};

