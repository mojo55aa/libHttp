#include "HTTPResponse.h"
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include "mojo_tools.h"

using namespace std;

#define KEY_ERROR "key not found"

#define MAX_INFLATE 10	// 最大gzip解压后的最大倍数，如果缓冲区控件依然不足则出错


HTTPResponse::HTTPResponse(const char* res_buff, unsigned int nsize)
{
	this->__buff_base = res_buff;
	this->__buff_size = nsize;
	this->status_code = 0;
	this->ok = false;
	this->content = nullptr;
	this->text = "";
	this->reason = "";
	this->content_size = 0;
	this->cookies = new CookieJar();
	this->_text_type.push_back("text");
	this->_text_type.push_back("json");
	this->_text_type.push_back("xml");
	this->_parseResponse(res_buff);
}


HTTPResponse::~HTTPResponse()
{
	// 如果是chunked方式解析的，则释放申请的临时内存
	if (this->_get("Transfer-Encoding") != KEY_ERROR)
	{
		delete[] this->content;
	}
	else // 不是chunked方式，则没有申请临时内存
	{
		this->content = nullptr;
	}
	delete this->cookies;
}

/**
 * @brief 保存二进制内容
 */
void HTTPResponse::SaveBinary(const char * filename)
{
	if (!this->ok || !this->content)
	{
		return;
	}
	ofstream outfile(filename, ios::out | ios::binary);
	outfile.write(this->content, this->content_size);
	outfile.close();
}

/**
 * @brief 保存字符串内容
 */
void HTTPResponse::SaveFile(const char * filename)
{
	if (this->text != "")
	{
		ofstream out;
		out.open(filename);
		out << this->text;
		out.close();
	}
}

/**
 * @brief 在headers中获取对应key的value,忽略大小写
 * @return 找到返回value string，没找到返回KEY_ERROR
 */
std::string HTTPResponse::_get(const string key) const
{
	// map<string, string>::const_iterator iter;
	for (auto iter = this->headers.begin(); iter != this->headers.end(); iter++)
	{
		if (this->__key_equal(iter->first, key))
		{
			return iter->second;
		}
	}
	return KEY_ERROR;
}

/**
 * @brief 解析HTTP Response header信息
 */
void HTTPResponse::_parseHeader(const string& header)
{
	vector<string> h_item;
	this->__split(header, h_item, "\r\n");
	if (h_item.size() == 0)
	{
		return;
	}
	// 解析header第一行
	vector<string> o;
	this->__split(h_item[0], o, " ");
	this->protocol = o[0];
	this->status_code = atoi(o[1].c_str());
	this->reason = o[2];
	for (unsigned int i = 3; i < o.size(); i++)
	{
		this->reason += " " + o[i];
	}
	if (o[1] == "200")
	{
		this->ok = true;
	}
	// 解析剩下的头信息
	for (unsigned int i = 1; i < h_item.size(); i++)
	{
		vector<string> tmp;
		this->__split(h_item[i], tmp, ": ");

		// set-cookie单独处理
		if (this->__key_equal(tmp[0], "set-cookie"))
		{
			this->cookies->cookies_extract(tmp[1]);
		}
		else
		{
			this->headers.insert(pair<string, string>(tmp[0], tmp[1]));
		}
	}
}

/**
 * @brief 解析HTTP Response body部分，主要针对Transfer-Encoding: chunked
 */
void HTTPResponse::_parseBody(const char * body)
{
	long body_size = 0;
	const char* ptr = body;
	// 申请一块临时内存
	char* tmp_mem = new char[this->__buff_size]();

	string line = this->_buffReadLine(ptr);
	int line_size = line.size() + 2;	// 加上最后的\r\n
	long chunk_size = strtol(line.c_str(), 0, 16);
	while (chunk_size != 0x0 && ptr <= this->__buff_base + this->__buff_size)
	{
		// 数据块复制到临时内存区
		memcpy(tmp_mem + body_size, ptr + line_size, chunk_size);

		body_size += chunk_size;

		// 获取下一个chunk size
		ptr += line_size + chunk_size + 2;	// 块结束还有一个\r\n
		line = this->_buffReadLine(ptr);
		line_size = line.size() + 2;
		chunk_size = strtol(line.c_str(), 0, 16);
	}

	// gzip压缩处理
	this->content_size = body_size;
	char* p_buff = tmp_mem;

	// 如果数据是gzip压缩
	if (this->_get("Content-Encoding") == "gzip")
	{
		int buff_len = body_size * MAX_INFLATE;
		char* uncompress_ = new char[buff_len]();
		int decode_ret = -1;

		decode_ret = mojo_tools::gzDecompress(tmp_mem, body_size, uncompress_, buff_len);

		if (decode_ret == -1)
		{// 解压出错，则返回
			delete[] uncompress_;
			delete[] tmp_mem;
			this->content_size = 0;
			return;
		}
		else
		{
			delete[] tmp_mem;
			p_buff = uncompress_;
			this->content_size = decode_ret;
		}
	}

	// 设置response结果
	if (this->_IsText())
	{
		this->text = string(p_buff, this->content_size);
		delete[] p_buff;
	}
	else
	{
		this->content = p_buff;
	}
}

/**
 * @brief 解析HTTP Response
 */
void HTTPResponse::_parseResponse(const char * response)
{
	string r(response);
	if (r.empty())
	{
		return;
	}
	// headers结束的位置
	int pos = r.find("\r\n\r\n");
	if (pos == string::npos)
	{
		return;
	}
	_parseHeader(r.substr(0, pos));


	// parseBody，针对chunked
	// HTTP/1.1协议规范只定义了chunked，所以header中如果有Transfer-Encoding，只考虑chunked
	if (this->_get("Transfer-Encoding") == KEY_ERROR)
	{
		// content直接指向response body部分
		this->content = (char*)(response + pos + 4);
		if (this->_IsText())
		{
			this->text = r.substr(pos + 4);
			this->content = nullptr;
		}

		// 直接设置成Content-Length
		this->content_size = atoi(this->_get("Content-Length").c_str());
		return;
	}
	else
	{// chunked解析
		this->_parseBody(response + pos + 4);
	}
}

/**
 * @brief content是否是text类型
 * @return bool
 */
bool HTTPResponse::_IsText() const
{
	// 遍历属于文本类型的vector
	for (auto item : this->_text_type)
	{
		if (this->_get("Content-Type").find(item) != string::npos)
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief 在一块buff中读取一行数据，转换成string.换行符\r\n
 * @buff buff地址
 * @return string
 */
std::string HTTPResponse::_buffReadLine(const char * buff) const
{
	unsigned int nsize = 0;
	const char* p = reinterpret_cast<const char*>(buff);
	while (p <= this->__buff_base + this->__buff_size)
	{
		if (*p == '\r')
		{
			if (*(p + 1) == '\n')
			{
				return string(buff, nsize);
			}
		}
		nsize += 1;
		p++;
	}
	return string(buff, nsize);
}

/**
 * @brief 字符串分割
 * @s 待分割的字符串
 * @v 分割后产生的vector<string> 数组
 * @sep 分割符
 */
void HTTPResponse::__split(const std::string & s, std::vector<std::string>& v, const std::string &sep) const
{
	int pos1, pos2;
	pos1 = s.find(sep);
	pos2 = 0;
	while (pos1 != string::npos)
	{
		v.push_back(s.substr(pos2, pos1 - pos2));
		pos2 = pos1 + sep.size();
		pos1 = s.find(sep, pos2);
	}
	if (pos2 != s.size())
	{
		v.push_back(s.substr(pos2));
	}
}

/**
 * @brief 忽略大小写比较
 */
bool HTTPResponse::__key_equal(const std::string &a, const std::string &b) const
{
	string _a, _b;
	_a.resize(a.size());
	_b.resize(b.size());
	transform(a.begin(), a.end(), _a.begin(), ::tolower);
	transform(b.begin(), b.end(), _b.begin(), ::tolower);
	if (_a == _b)
	{
		return true;
	}
	return false;
}
