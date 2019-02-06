
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <map>
#include "lib_socket.h"
#include "CPPRequests.h"
#include "HTTPSession.h"
using namespace std;

#define MAX_RECV_BUFF 0x7ffff	//512KB


void test_requests()
{
	CPPRequests* requests = new CPPRequests();
	map<string, string> data, headers;
	//headers["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8";
	headers["Accept-Encoding"] = "gzip, deflate, br";
	//headers["Upgrade-Insecure-Requests"] = "1";
	//headers["Accept-Language"] = "zh-CN,zh;q=0.9";
	//headers["content-type"] = "application/x-www-form-urlencoded; charset=UTF-8";
	//headers["content-length"] = "73";
	data["userId"] = "0";
	data["cityId"] = "19";
	data["shopType"] = "0";
	data["categoryId"] = "116";
	data["viewShopId"] = "21800848";
	data["pageId"] = "2";
	string str_data = "userId=0&cityId=19&shopType=0&categoryId=116&viewShopId=21800848&pageId=2";
	string url = "https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&tn=baidu&wd=tcp%20400%20The%20plain%20HTTP%20request%20was%20sent%20to%20HTTPS%20port&oq=400%2520The%2520plain%2520HTTP%2520request%2520was%2520sent%2520to%2520HTTPS%2520port&rsv_pq=c47a22cc000343ec&rsv_t=f9878krFzFCXmkwZfpD6n4rU6fRpd0e%2FQeiIGdY1RC0wOJy18QsI7BUFUwc&rqlang=cn&rsv_enter=1&inputT=3077&rsv_sug3=28&rsv_sug2=0&rsv_sug4=3971";
	
	HTTPResponse* httpResponse;
	httpResponse = requests->Get("http://127.0.0.1:8000/login", "", &headers);
	cout << httpResponse->cookies->cookies_for_server() << endl;
	httpResponse->SaveFile("C:\\Users\\MOJO\\Desktop\\res_str.html");
	httpResponse->SaveBinary("C:\\Users\\MOJO\\Desktop\\lgog.png");
	// https://blog.csdn.net/sun223173/article/details/80631252
	// http://site.ip138.com/domain/write.do?input=bbs.csdn.net&token=931f1f3fcf87317a8dd60ce4035eb4f5
	// https://www.baidu.com/img/bd_logo1.png
}


void test_session()
{
	map<string, string> data, headers;

	string url = "https://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&rsv_idx=1&tn=baidu&wd=tcp%20400%20The%20plain%20HTTP%20request%20was%20sent%20to%20HTTPS%20port&oq=400%2520The%2520plain%2520HTTP%2520request%2520was%2520sent%2520to%2520HTTPS%2520port&rsv_pq=c47a22cc000343ec&rsv_t=f9878krFzFCXmkwZfpD6n4rU6fRpd0e%2FQeiIGdY1RC0wOJy18QsI7BUFUwc&rqlang=cn&rsv_enter=1&inputT=3077&rsv_sug3=28&rsv_sug2=0&rsv_sug4=3971";

	HTTPSession* session = new HTTPSession();
	HTTPResponse* httpResponse;

	headers["Accept-Encoding"] = "gzip, deflate, br";
	data["username"] = "MSY001";
	data["password"] = "000000";

	httpResponse = session->Post("http://127.0.0.1:8000/login", &data, &headers);

	cout << httpResponse->cookies->cookies_for_server() << endl;
	httpResponse = session->Get("http://127.0.0.1:8000/index", "", &headers);

	// httpResponse = session->Get(url.c_str(), "", &headers);
	// httpResponse = session->Get(url.c_str(), "", &headers);
	httpResponse->SaveFile("C:\\Users\\MOJO\\Desktop\\res_str.html");
}

int main()
{
	// test_requests();
	test_session();

	system("pause");
}



#if 0
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "zlib.h"


// gzCompress: do the compressing
int gzCompress(const char *src, int srcLen, char *dest, int destLen)
{
	z_stream c_stream;
	int err = 0;
	int windowBits = 15;
	int GZIP_ENCODING = 16;

	if (src && srcLen > 0)
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = (Bytef *)src;
		c_stream.avail_in = srcLen;
		c_stream.next_out = (Bytef *)dest;
		c_stream.avail_out = destLen;
		while (c_stream.avail_in != 0 && c_stream.total_out < destLen)
		{
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		return c_stream.total_out;
	}
	return -1;
}

// gzDecompress: do the decompressing
int gzDecompress(const char *src, int srcLen, const char *dst, int dstLen) {
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
			return err;
		}
	}
	else {
		inflateEnd(&strm);
		return err;
	}
	inflateEnd(&strm);
	return err;
}

int main()
{
	const char* src = "just for test, dd, dd, dd";
	int size_src = strlen(src);
	char* compressed = (char*)malloc(size_src * 2);
	memset(compressed, 0, size_src * 2);
	printf("to compress src: %s\n", src);
	printf("to compress src size: %d\n", size_src);

	int gzSize = gzCompress(src, size_src, compressed, size_src * 2);
	if (gzSize <= 0)
	{
		printf("compress error.\n");
		return -1;
	}
	printf("compressed: ");
	int i = 0;
	for (; i < gzSize; ++i)
	{
		printf("%02x ", compressed[i]);
	}
	printf("\ncompressed size: %d\n", gzSize);

	char* uncompressed = (char*)malloc(size_src * 2);
	memset(uncompressed, 0, size_src * 2);
	int ret = gzDecompress(compressed, gzSize, uncompressed, size_src * 2);
	printf("uncompressed: %s\n", uncompressed);
	printf("uncompressed size: %d\n", strlen(uncompressed));

	free(compressed);
	free(uncompressed);
	system("pause");
	return 0;
}
#endif