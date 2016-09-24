#pragma once

class CDirectSoundBuffer
{
public:
	CDirectSoundBuffer(void);
	~CDirectSoundBuffer(void);

	VOID	Clear			();
	VOID	Set				(PVOID pBuffer, ULONG Length);
	PUCHAR	GetBuffer		();
	ULONG	GetLength		();

private:
	PUCHAR	m_pBuffer;
	ULONG	m_Length;
	ULONG	m_BufferLength;	
};

class CDirectSoundBufferPool
{
public:
	CDirectSoundBufferPool ();
	~CDirectSoundBufferPool ();

	CDirectSoundBuffer*	Alloc		();
	CDirectSoundBuffer*	Alloc		(PVOID pBuffer, ULONG Length);
	VOID		Free		(CDirectSoundBuffer* pBuffer);

private:
	CList<CDirectSoundBuffer*>		m_FreeList;
	CCriticalSection	m_ListLock;
};