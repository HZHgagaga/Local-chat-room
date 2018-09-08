#ifndef __LIST_H__
#define __LIST_H__

struct ADDR
{
	sockaddr_in m_addr;

	ADDR *m_pNext;
	ADDR *m_pLast;
};

#endif