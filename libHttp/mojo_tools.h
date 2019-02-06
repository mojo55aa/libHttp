#pragma once
#include <string>
#include <vector>

namespace mojo_tools {

	void buff_to_file(const char* buff, const char* filename);

	std::string& trim(std::string &s);
	std::string& ltrim(std::string &s);
	std::string& rtrim(std::string &s);

	void split(const std::string& s, std::vector<std::string>& v, const std::string& sep);
	std::string lower(const std::string &s);

	// gzip uncompress
	int gzDecompress(const char *src, int srcLen, const char *dst, int dstLen);
}
