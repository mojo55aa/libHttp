#pragma once
#include <list>


typedef struct memnode_t
{
	char* p_data;				// �������ַ
	unsigned int free_cnt;		// ��ǰ�ڵ�����ֽ���
	bool full;					// �Ƿ��Ѿ�����
	char* p_avail;				// ���ÿռ俪ʼ��ַ
	char* p_end;				// ���ÿռ������ַ
}memnode;

class MemoryAutoAdapter
{
public:
	MemoryAutoAdapter();
	virtual ~MemoryAutoAdapter();

	unsigned int write(const char* src, unsigned int size = 0);
	unsigned int append(const char* src, unsigned int size = 0);
	unsigned int read(char* dst, unsigned int size = 0xffffffff);
	/* �ӻ������ж�ȡ���ݣ�ͬʱɾ���Ѿ����������� */
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
	std::list<memnode> __store;		// ����������
	unsigned int __page_size;		// �������ڵ�����������
	unsigned int __Bytes;			// ��������ǰ�洢�����ֽ���
	unsigned int __GetOffset;		// get����������ƫ��
};

