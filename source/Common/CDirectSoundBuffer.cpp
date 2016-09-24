#include "StdAfx.h"
#include "CDirectSoundBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDirectSoundBuffer::CDirectSoundBuffer ()
{
	m_Length		= 0;
	m_BufferLength	= 0;
	m_pBuffer		= NULL;
}

CDirectSoundBuffer::~CDirectSoundBuffer ()
{
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
}

VOID CDirectSoundBuffer::Clear ()
{
	::ZeroMemory (m_pBuffer, m_BufferLength);
	m_Length = 0;
}

VOID CDirectSoundBuffer::Set (PVOID pBuffer, ULONG Length)
{
	if (m_BufferLength < Length)
	{
		m_BufferLength = (Length + 255) & ~255;
		if (m_pBuffer != NULL)
		{
			delete m_pBuffer;
		}
		m_pBuffer = new UCHAR[m_BufferLength];
	}

	::ZeroMemory (m_pBuffer, m_BufferLength);

	m_Length = Length;
	memcpy (m_pBuffer, pBuffer, Length);
}

PUCHAR CDirectSoundBuffer::GetBuffer ()
{
	return m_pBuffer;
}

ULONG CDirectSoundBuffer::GetLength ()
{
	return m_Length;
}






CDirectSoundBufferPool::CDirectSoundBufferPool ()
{
}

CDirectSoundBufferPool::~CDirectSoundBufferPool ()
{
	while (!m_FreeList.IsEmpty ())
	{
		delete m_FreeList.RemoveHead ();
	}
}

CDirectSoundBuffer* CDirectSoundBufferPool::Alloc ()
{
	CDirectSoundBuffer* pNewBuffer = NULL;

	m_ListLock.Lock ();
	if (m_FreeList.IsEmpty ())
	{
		pNewBuffer = new CDirectSoundBuffer;
	}
	else
	{
		pNewBuffer = m_FreeList.RemoveHead ();
	}
	m_ListLock.Unlock ();

	pNewBuffer->Clear ();

	return pNewBuffer;
}

CDirectSoundBuffer* CDirectSoundBufferPool::Alloc (PVOID pBuffer, ULONG Length)
{
	CDirectSoundBuffer* pNewBuffer = Alloc ();

	pNewBuffer->Set (pBuffer, Length);

	return pNewBuffer;
}

VOID CDirectSoundBufferPool::Free (CDirectSoundBuffer* pBuffer)
{
	m_ListLock.Lock ();
	m_FreeList.AddTail (pBuffer);
	m_ListLock.Unlock ();
}