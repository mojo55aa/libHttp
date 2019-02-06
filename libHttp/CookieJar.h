#pragma once
#include <string>
#include <vector>

typedef struct Cookie
{
	std::string name;
	std::string value;
	std::string domain;
	std::string path;

	std::string expires;
	bool secure;
	bool httpOnly;
}CookieUnit;


class CookieJar
{
public:
	CookieJar();
	virtual ~CookieJar();

	std::string cookies_for_server() const;
	void set_cookie(std::string name, std::string value);
	void set_cookie(CookieUnit *cookie);
	void remove_cookie(std::string name);
	void clear();
	// void cookies_extract(HTTPResponse* response);
	void cookies_extract(std::string _set_cookie);

	std::vector<CookieUnit> get_cookies();
protected:
	int _cookies_is_exist(std::string name) const;

private:
	std::vector<CookieUnit> cookies;

private:
	void __split(const std::string & s, std::vector<std::string>& v, const std::string &sep) const;
	std::string __lower(const std::string &s) const;
};

