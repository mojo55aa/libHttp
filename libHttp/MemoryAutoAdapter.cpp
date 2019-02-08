#include "MemoryAutoAdapter.h"


#define DEFAULT_PAGE_SIZE 0x20000	// Ĭ��һ���ڴ���Ĵ�С128KB


MemoryAutoAdapter::MemoryAutoAdapter()
{
	this->__page_size = DEFAULT_PAGE_SIZE;
	this->__Bytes = 0;
}


MemoryAutoAdapter::~MemoryAutoAdapter()
{
	/* ���������ͷ��ڴ� */
	for (auto iter = this->__store.begin(); iter != __store.end(); iter++)
	{
		this->_node_free(&*iter);
	}
}

/**
 * @brief �򻺳�����д�����ݣ��ӻ�����ͷ����ʼд�����������򸲸�
 * @src Դ�ڴ��ַ
 * @size д����ֽ���
 * @return ����ʵ��д����ֽ���
 */
unsigned int MemoryAutoAdapter::write(const char * src, unsigned int size)
{
	// ���ȴ�ͷ��������д������
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
	// �����еĽڵ������������ӽڵ�
	while (size > this->__Bytes)
	{
		this->_alloter();
		memnode& node = this->__store.back();
		this->__Bytes += this->_node_write(&node, src + this->__Bytes, size - this->__Bytes);
	}
	return this->__Bytes;
}

/**
 * @brief �򻺳�����׷������
 * @return ʵ��д����ֽ���
 */
unsigned int MemoryAutoAdapter::append(const char * src, unsigned int size)
{
	unsigned int _leftBytes = size;
	for (auto iter = this->__store.begin(); iter != this->__store.end(); iter++)
	{
		//TODO ���������⣬���free_cnt���ٽ�ֵ�����������ϴε���������
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
	// �ж��Ƿ�д��
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
 * @brief �ӻ������ж�ȡ����
 * @dst Ŀ���ڴ��ַ
 * @size ��ȡ���ֽ���
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
 * @brief ���û�����һ���ڵ�Ĵ�С����֤��ֵ��2^n
 * return ʵ�����õĴ�С
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
 * @brief ���ػ���������ʹ�õ��ֽ���
 */
unsigned int MemoryAutoAdapter::size() const
{
	return this->__Bytes;
}

/**
 * @brief ����һ���ڵ㲢��ʼ�������뵽�����ĩβ
 */
void MemoryAutoAdapter::_alloter()
{
	memnode node;
	node.p_data = new char[this->__page_size]();
	this->_node_init(&node);

	this->__store.push_back(node);
}

/**
 * @brief ����һ���ڵ㣬��������ָ����ڴ���գ����������ָ�Ĭ��
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
 * @brief �ͷ�һ���ڵ��������
 */
void MemoryAutoAdapter::_node_free(memnode * node)
{
	delete[] node->p_data;
}

/**
 * @brief ��ڵ���д������
 * @node д��Ľڵ�
 * @src Դ���ݵ�ַ
 * @size д����ֽ���
 * @return ����ʵ��д����ֽ���
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
 * @brief �ӽڵ��ж�ȡ����
 * @node ��ȡ�Ľڵ�
 * @dst Ŀ���ڴ��ַ
 * @size ��ȡ���ֽ���
 */
unsigned int MemoryAutoAdapter::_node_read(memnode * node, char * dst, unsigned int size)
{
	unsigned int _rSize = size <= (this->__page_size - node->free_cnt) ? size : (this->__page_size - node->free_cnt);
	memcpy(dst, node->p_data, _rSize);
	return _rSize;
}
