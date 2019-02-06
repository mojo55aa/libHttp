#include "CookieJar.h"
#include <algorithm>
#include "mojo_tools.h"

using namespace std;



CookieJar::CookieJar()
{
}


CookieJar::~CookieJar()
{
}

/**
 * @brief 返回所有cookies拼接后的字符串，用于再HTTP Request中发送
 */
std::string CookieJar::cookies_for_server() const
{
	string c = "";
	for (auto item : this->cookies)
	{
		c += "; " + item.name + "=" + item.value;
	}
	c.erase(0, 2);
	return c;
}

/**
 * @brief 向vector中添加cookie，根据name和value，其他字段默认
 * 如果name存在，则更新value
 */
void CookieJar::set_cookie(string name,string value)
{
	int idx = this->_cookies_is_exist(name);
	if (idx == -1)
	{
		CookieUnit cookie;
		cookie.name = name;
		cookie.value = value;
		cookie.domain = "";
		cookie.expires = "";
		cookie.httpOnly = false;
		cookie.path = "/";
		cookie.secure = false;
		this->cookies.push_back(cookie);
	}
	else
	{
		this->cookies[idx].value = value;
	}
}

/**
 * @brief 向vector中添加cookie，如果已经存在相同的name，则先删除再添加
 */
void CookieJar::set_cookie(CookieUnit *cookie)
{
	int idx = this->_cookies_is_exist(cookie->name);
	if (idx == -1)
	{
		this->cookies.push_back(*cookie);
	}
	else
	{
		this->cookies.erase(this->cookies.begin() + idx);
		this->cookies.push_back(*cookie);
	}
}

/**
 * @brief 从vector中删除对应name的cookie
 */
void CookieJar::remove_cookie(string name)
{
	int idx = this->_cookies_is_exist(name);
	if (idx != -1)
	{
		this->cookies.erase(this->cookies.begin() + idx);
	}
}

/**
 * @brief 删除所有cookies
 */
void CookieJar::clear()
{
	this->cookies.clear();
}


/**
 * @brief 扩展HTTP Response 中的set-cookie字段
 */
void CookieJar::cookies_extract(string _set_cookie)
{
	CookieUnit cookie;
	cookie.name = "";
	cookie.value = "";
	cookie.domain = "";
	cookie.expires = "";
	cookie.httpOnly = false;
	cookie.path = "/";
	cookie.secure = false;

	vector<string> item;
	this->__split(_set_cookie, item, ";");

	vector<string> c;
	this->__split(item[0], c, "=");
	// 去头部空格添加
	cookie.name = mojo_tools::ltrim(c[0]);
	// 应对cookie=;情况
	if (c.size() > 1)
	{
		cookie.value = c[1];
		// value中可能还有=
		for (size_t i = 2; i < c.size(); i++)
		{
			cookie.value += "=" + c[i];
		}
	}
	else
	{
		cookie.value = "";
	}
	
	for (size_t i = 1; i < item.size(); i++)
	{
		vector<string> tmp;
		this->__split(item[i], tmp, "=");
		// 转换小写
		string _k = this->__lower(tmp[0]);
		// 去掉左边的空格
		mojo_tools::ltrim(_k);
		if (_k == "httponly")
		{
			cookie.httpOnly = true;
		}
		else if (_k == "domain")
		{
			cookie.domain = tmp[1];
		}
		else if (_k == "expires")
		{
			cookie.expires = tmp[1];
		}
		else if (_k == "path")
		{
			cookie.path = tmp[1];
		}
		else if (_k == "secure")
		{
			cookie.secure = true;
		}
	}

	this->set_cookie(&cookie);
}

 vector<CookieUnit> CookieJar::get_cookies()
 {
	 return this->cookies;
 }

/**
 * @brief 检查对应name的cookie是否已经存在
 * @return 存在返回在vector中的下标，不存在返回-1
 */
int CookieJar::_cookies_is_exist(string name) const
{
	for (size_t idx = 0; idx < this->cookies.size(); idx++)
	{
		if (this->cookies[idx].name == name)
		{
			return idx;
		}
	}
	return -1;
}


/**
 * @brief 字符串分割
 * @s 待分割的字符串
 * @v 分割后产生的vector<string> 数组
 * @sep 分割符
 */
void CookieJar::__split(const std::string & s, std::vector<std::string>& v, const std::string &sep) const
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
 * @brief string转换小写
 */
std::string CookieJar::__lower(const std::string &s) const
{
	string t;
	t.resize(s.size());
	transform(s.begin(), s.end(), t.begin(), ::tolower);
	return t;
}
