#pragma once
#include <list>


typedef struct memnode_t
{
	char* p_data;				// 数据域地址
	unsigned int free_cnt;		// 当前节点空闲字节数
	bool full;					// 是否已经存满
	char* p_avail;				// 可用空间开始地址
	char* p_end;				// 可用空间结束地址
}memnode;

class MemoryAutoAdapter
{
public:
	MemoryAutoAdapter();
	virtual ~MemoryAutoAdapter();

	unsigned int write(const char* src, unsigned int size = 0);
	unsigned int append(const char* src, unsigned int size = 0);
	unsigned int read(char* dst, unsigned int size = 0xffffffff);
	/* 从缓冲区中读取数据，同时删除已经读出的数据 */
	unsigned int get(char* dst, unsigned int size = 0xffffffff);

	unsigned int setPageSize(unsigned int size);
	unsigned int size() const;

protected:
	void _alloter();
	void _node_init(memnode* node);
	void _node_free(memnode* node);
	unsigned int _node_write(memnode* node, const char* src, unsigned int size);
	unsigned int _node_read(memnode* node, char* dst, unsigned int size);

private:
	std::list<memnode> __store;		// 缓冲区链表
	unsigned int __page_size;		// 缓冲区节点数据域容量
	unsigned int __Bytes;			// 缓冲区当前存储数据字节数
	unsigned int __GetOffset;		// get函数的数据偏移
};

