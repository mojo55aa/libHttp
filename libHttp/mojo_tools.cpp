#include "mojo_tools.h"
#include <fstream>
#include <algorithm>

#ifndef __LINUX__
// WIN平台使用目录下的zlib静态库zlibstat.lib
#include "zlib.h"
#else
// linux下使用系统安装的zlib
#include <zlib/zlib.h>
#endif // !__LINUX__

using namespace std;

namespace mojo_tools {

	void buff_to_file(const char * buff, const char * filename)
	{
		string s(buff);
		ofstream file;
		file.open(filename);
		file << s << endl;
		file.close();
	}


	string& trim(string &s)
	{
		if (s.empty())
		{
			return s;
		}
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}

	string& ltrim(string &s)
	{
		if (s.empty())
		{
			return s;
		}
		s.erase(0, s.find_first_not_of(" "));
		return s;
	}

	string& rtrim(string &s)
	{
		if (s.empty())
		{
			return s;
		}
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}

	 /**
	 * @brief 字符串分割
	 * @s 待分割的字符串
	 * @v 分割后产生的vector<string> 数组
	 * @sep 分割符
	 */
	void split(const string & s, vector<string>& v, const string &sep)
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
	string lower(const string &s)
	{
		string t;
		t.resize(s.size());
		transform(s.begin(), s.end(), t.begin(), ::tolower);
		return t;
	}


	// gzDecompress: do the decompressing
	int gzDecompress(const char *src, int srcLen, const char *dst, int dstLen)
	{
		z_stream strm;
		strm.zalloc = NULL;
		strm.zfree = NULL;
		strm.opaque = NULL;

		strm.avail_in = srcLen;
		strm.avail_out = dstLen;
		strm.next_in = (Bytef *)src;
		strm.next_out = (Bytef *)dst;

		int err = -1, ret = -1;
		err = inflateInit2(&strm, MAX_WBITS + 16);
		if (err == Z_OK) {
			err = inflate(&strm, Z_FINISH);
			if (err == Z_STREAM_END) {
				ret = strm.total_out;
			}
			else {
				inflateEnd(&strm);
				return -1;
			}
		}
		else {
			inflateEnd(&strm);
			return -1;
		}
		inflateEnd(&strm);
		return ret;
	}
}
