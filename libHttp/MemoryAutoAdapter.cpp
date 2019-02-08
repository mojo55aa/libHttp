#include "MemoryAutoAdapter.h"


#define DEFAULT_PAGE_SIZE 0x20000	// 默认一个内存结点的大小128KB


MemoryAutoAdapter::MemoryAutoAdapter()
{
	this->__page_size = DEFAULT_PAGE_SIZE;
	this->__Bytes = 0;
}


MemoryAutoAdapter::~MemoryAutoAdapter()
{
	/* 遍历链表，释放内存 */
	for (auto iter = this->__store.begin(); iter != __store.end(); iter++)
	{
		this->_node_free(&*iter);
	}
}

/**
 * @brief 向缓冲区中写入数据，从缓冲区头部开始写，存在数据则覆盖
 * @src 源内存地址
 * @size 写入的字节数
 * @return 返回实际写入的字节数
 */
unsigned int MemoryAutoAdapter::write(const char * src, unsigned int size)
{
	// 首先从头遍历链表，写入数据
	for (auto iter = this->__store.begin(); iter != this->__store.end(); iter++)
	{
		if (this->__Bytes + this->__page_size <= size)
		{
			int _wSize = this->_node_write(&*iter, src + this->__Bytes, this->__page_size);
			this->__Bytes += _wSize;
		}
		else
		{
			int _wSize = this->_node_write(&*iter, src + this->__Bytes, size - this->__Bytes);
			this->__Bytes += _wSize;
			break;
		}
	}
	// 链表中的节点如果不够，添加节点
	while (size > this->__Bytes)
	{
		this->_alloter();
		memnode& node = this->__store.back();
		this->__Bytes += this->_node_write(&node, src + this->__Bytes, size - this->__Bytes);
	}
	return this->__Bytes;
}

/**
 * @brief 向缓冲区中追加数据
 * @return 实际写入的字节数
 */
unsigned int MemoryAutoAdapter::append(const char * src, unsigned int size)
{
	unsigned int _leftBytes = size;
	for (auto iter = this->__store.begin(); iter != this->__store.end(); iter++)
	{
		//TODO 可能有问题，如果free_cnt是临界值，可能增加上次的垃圾数据
		if (iter->full)
		{
			continue;
		}
		_leftBytes -= this->_node_write(&*iter, src + (size - _leftBytes), _leftBytes);
		if (!_leftBytes)
		{
			break;
		}
	}
	// 判断是否写完
	while (_leftBytes)
	{
		this->_alloter();
		memnode& node = this->__store.back();
		_leftBytes -= this->_node_write(&node, src + (size - _leftBytes), _leftBytes);
	}
	this->__Bytes += size;
	return this->__Bytes;
}

/**
 * @brief 从缓冲区中读取数据
 * @dst 目标内存地址
 * @size 读取的字节数
 */
unsigned int MemoryAutoAdapter::read(char * dst, unsigned int size)
{
	if (this->__Bytes == 0)
	{
		return 0;
	}

	size = (size <= this->__Bytes) ? size : this->__Bytes;
	unsigned int _rSize = 0;
	while (size - _rSize > 0)
	{
		for (auto iter = this->__store.begin(); iter != this->__store.end(); iter++)
		{
			_rSize += this->_node_read(&*iter, dst + _rSize, size - _rSize);
		}
	}
	return _rSize;
}

/**
 * @brief 设置缓冲区一个节点的大小，保证数值是2^n
 * return 实际设置的大小
 */
unsigned int MemoryAutoAdapter::setPageSize(unsigned int size)
{
	int x = 0;
	unsigned int tmp = size;
	while (tmp > 1)
	{
		tmp >>= 1;
		x++;
	}
	x = (size & (size - 1)) ? x + 1 : x;
	this->__page_size = 1 << x;
	return (1 << x);
}

/**
 * @brief 返回缓冲区中已使用的字节数
 */
unsigned int MemoryAutoAdapter::size() const
{
	return this->__Bytes;
}

/**
 * @brief 申请一个节点并初始化，插入到链表的末尾
 */
void MemoryAutoAdapter::_alloter()
{
	memnode node;
	node.p_data = new char[this->__page_size]();
	this->_node_init(&node);

	this->__store.push_back(node);
}

/**
 * @brief 重置一个节点，将数据域指向的内存清空，其他变量恢复默认
 */
void MemoryAutoAdapter::_node_init(memnode * node)
{
	memset(node->p_data, 0, this->__page_size);
	node->free_cnt = this->__page_size;
	node->full = false;
	node->p_avail = node->p_data;
	node->p_end = node->p_data + this->__page_size;
}

/**
 * @brief 释放一个节点的数据域
 */
void MemoryAutoAdapter::_node_free(memnode * node)
{
	delete[] node->p_data;
}

/**
 * @brief 向节点中写入数据
 * @node 写入的节点
 * @src 源数据地址
 * @size 写入的字节数
 * @return 返回实际写入的字节数
 */
unsigned int MemoryAutoAdapter::_node_write(memnode * node, const char * src, unsigned int size)
{
	if (node->free_cnt == 0)
	{
		return 0;
	}
	if (size >= node->free_cnt)
	{
		memcpy(node->p_avail, src, node->free_cnt);
		node->full = true;
		node->p_avail += node->free_cnt;
		int _wSize = node->free_cnt;
		node->free_cnt = 0;
		return _wSize;
	}
	else
	{
		memcpy(node->p_avail, src, size);
		node->p_avail += size;
		node->free_cnt -= size;
		return size;
	}
}

/**
 * @brief 从节点中读取数据
 * @node 读取的节点
 * @dst 目标内存地址
 * @size 读取的字节数
 */
unsigned int MemoryAutoAdapter::_node_read(memnode * node, char * dst, unsigned int size)
{
	unsigned int _rSize = size <= (this->__page_size - node->free_cnt) ? size : (this->__page_size - node->free_cnt);
	memcpy(dst, node->p_data, _rSize);
	return _rSize;
}
