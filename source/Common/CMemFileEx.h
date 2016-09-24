#pragma once
#include "afxmt.h"
#include "afxtempl.h"

class CMemFileEx: public CMemFile
{
public:
	CMemFileEx();
	virtual ~CMemFileEx();

	PUCHAR GetBuffer ()
	{
		return m_lpBuffer;
	}

	ULONG GetLength ()
	{
		return static_cast<ULONG>(CMemFile::GetLength ());
	}

	CCriticalSection* GetLock ()
	{
		return &m_Lock;
	}

	BOOL Lock()
	{
		return m_Lock.Lock ();
	}
	
	BOOL Unlock()
	{
		return m_Lock.Unlock ();
	}

	VOID Write (PVOID pBuffer, ULONG Length)
	{
		CSingleLock	Lock (&m_Lock, TRUE);

		CMemFile::Write (pBuffer, Length);
	}

protected:
	CCriticalSection	m_Lock;
};


class CMemFileExList
{
public:
	CMemFileExList ()
	{
	}

	~CMemFileExList ()
	{
		while (!m_UsedList.IsEmpty ())
		{
			delete m_UsedList.RemoveHead ();
		}
		while (!m_FreeList.IsEmpty ())
		{
			delete m_FreeList.RemoveHead ();
		}
	}

	CCriticalSection* GetLock ()
	{
		return &m_Lock;
	}

	BOOL Lock ()
	{
		return m_Lock.Lock ();
	}

	BOOL Unlock ()
	{
		return m_Lock.Unlock ();
	}

	VOID RemoveAll ()
	{
		CSingleLock		Lock (&m_Lock, TRUE);
	
		while (!m_UsedList.IsEmpty ())
		{
			m_FreeList.AddTail (m_UsedList.RemoveHead ());
		}
	}

	INT_PTR GetUsedMemFiles (CList<CMemFileEx*>* pList)
	{
		CSingleLock		Lock (&m_Lock, TRUE);
		
		while (!m_UsedList.IsEmpty ())
		{
			pList->AddTail (m_UsedList.RemoveHead ());
		}

		return pList->GetCount ();
	}

	CMemFileEx* GetFreeMemFile ()
	{
		CSingleLock		Lock (&m_Lock, TRUE);
		CMemFileEx*		pMemFile = NULL;

		do
		{
			if (!m_FreeList.IsEmpty ())
			{
				pMemFile = m_FreeList.RemoveHead ();
				break;
			}

			pMemFile = new CMemFileEx;
		}
		while (0);

		pMemFile->SetLength (0);

		return pMemFile;
	}

	VOID UsedMemFile (CMemFileEx* pMemFile)
	{
		CSingleLock		Lock (&m_Lock, TRUE);
		m_UsedList.AddTail (pMemFile);
	}

	VOID FreeMemFile (CMemFileEx* pMemFile)
	{
		CSingleLock		Lock (&m_Lock, TRUE);
		m_FreeList.AddTail (pMemFile);
	}

protected:
	CCriticalSection	m_Lock;
	CList<CMemFileEx*>	m_UsedList;
	CList<CMemFileEx*>	m_FreeList;
};