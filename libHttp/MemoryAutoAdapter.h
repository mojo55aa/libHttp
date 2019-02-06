#pragma once
#include <list>


typedef struct memnode_t
{
	char* data;
	unsigned int used;
	bool full;
}memnode;

class MemoryAutoAdapter
{
public:
	MemoryAutoAdapter();
	virtual ~MemoryAutoAdapter();

	int write(const char* src, unsigned int size=0);
	int read(char* dst, unsigned int size=0);

	void setPageSize(unsigned int size);

protected:
	void _alloter();

private:
	std::list<memnode> __store;
	unsigned int __page_size;
};

