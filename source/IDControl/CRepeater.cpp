#include "stdafx.h"
#include "CRepeater.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CRepeater::CRepeater()
{
}


CRepeater::~CRepeater()
{
}

VOID CRepeater::Set (CStringArray* pArray)
{
//Group No,Group Name,Name,Sub Name,Repeater Call Sign,Gateway Call Sign,Frequency,Dup,Offset,Mode,TONE,Repeater Tone,RPT1USE,Position,Latitude,Longitude,UTC Offset
//1,ŠÖ“Œ,H—tŒ´430,“Œ‹ž“s,JP1YLA A,JP1YLA G,434.320000,DUP+,5.000000,DV,OFF,88.5Hz,YES,Approximate,35.697750,139.772150,+9:00
	do
	{
		if (pArray->GetCount () < 13)
		{	break;
		}

		m_Group			= _ttol (pArray->GetAt (0));
		m_strGroupName	= pArray->GetAt (1);
		m_strName		= pArray->GetAt (2);
		m_strSubName	= pArray->GetAt (3);
		m_strCallsign	= pArray->GetAt (4);
		m_strToCallsign	= GetToCallsign (m_strCallsign);
		m_strGateway	= pArray->GetAt (5);
		m_strFrequency	= pArray->GetAt (6);
		m_Frequency		= static_cast<ULONGLONG>(_ttof (pArray->GetAt (6)) * 100) * 10000;
		m_Dup			= (pArray->GetAt (7) == _T("DUP+")) ? 0x12 : (pArray->GetAt (7) == _T("DUP-")) ? 0x11 : 0x10; 
		m_Offset		= static_cast<ULONGLONG>(_ttof (pArray->GetAt (8)) * 100) * 10000;
		m_strOffset		= pArray->GetAt (8);
		m_DvMode		= pArray->GetAt (9) == _T("DV");
		m_ToneEnable	= pArray->GetAt (10) == _T("ON");
		m_strRepeaterTone= pArray->GetAt (11);
		m_RepeaterTone	= static_cast<ULONG>(_ttof (pArray->GetAt (11)) * 10);
		m_Rpt1Use		= pArray->GetAt (12) == _T("YES");

		//m_strDisplay.Format (_T("%02u : %s"), GroupNo, pArray->GetAt (1));

		//if (!m_strCallsign.IsEmpty ())
		//{
		//	strDisplay.AppendFormat (_T(" (%s)"), Array.GetAt (4));
		//}

	}
	while (0);
}

CString CRepeater::GetToCallsign (CString strCallsign)
{
	strCallsign.Trim ();
	strCallsign = _T("/") + strCallsign;
	if (strCallsign.GetLength () > 8)
	{
		strCallsign = strCallsign.Left (7) + strCallsign.Right (1);
	}

	return strCallsign;
}


CRepeaterList::CRepeaterList ()
{
}

CRepeaterList::~CRepeaterList ()
{
	Clear ();
}

VOID CRepeaterList::Clear ()
{
	POSITION	Pos;
	CRepeater*	pRepeater;
	CString		strCallsign;

	Pos = m_Map.GetStartPosition ();
	while (Pos != NULL)
	{
		m_Map.GetNextAssoc (Pos, strCallsign, (void*&)pRepeater);
		DBG_MSG((_T("CRepeaterList::Clear - %p, %s, %s\n"), pRepeater, pRepeater->GetCallsign (), pRepeater->GetName ()));

		delete pRepeater;
	}

	m_Map.RemoveAll ();
	m_List.RemoveAll ();
}

VOID CRepeaterList::RemoveAt (CRepeater* pRemoveRepeater)
{
	POSITION	Pos;
	CRepeater*	pRepeater;
	CString		strCallsign;

	DBG_MSG((_T("CRepeaterList::RemoveAt - %p, %s, %s\n"), pRemoveRepeater, pRemoveRepeater->GetCallsign (), pRemoveRepeater->GetName ()));

	if (::lstrlen (pRemoveRepeater->GetCallsign ()) == 0)
	{
		CString strFreq;
		strFreq.Format (_T("%I64u"), pRemoveRepeater->GetFrequency ());
		m_Map.RemoveKey (strFreq);
	}
	else
	{
		m_Map.RemoveKey (pRemoveRepeater->GetCallsign ());
	}

	Pos = m_List.GetHeadPosition ();
	while (Pos != NULL)
	{
		pRepeater = m_List.GetAt (Pos);
		
		if (pRemoveRepeater == pRepeater)
		{
			m_List.RemoveAt(Pos);
			break;
		}

		m_List.GetNext (Pos);
	}

	delete pRemoveRepeater;
}

BOOL CRepeaterList::Lookup (LPCTSTR pszCallSign, CRepeater*& pResultRepeater)
{
	POSITION	Pos;
	CRepeater*	pRepeater;
	BOOL		Result	 = FALSE;

	Pos = m_List.GetHeadPosition ();
	while (Pos != NULL)
	{
		pRepeater = m_List.GetAt (Pos);
		
		if (lstrcmp (pszCallSign, pRepeater->GetCallsign ()) == 0)
		{
			Result = TRUE;
			pResultRepeater = pRepeater;
			break;
		}

		m_List.GetNext (Pos);
	}

	return Result;
}

BOOL CRepeaterList::LookupTo (LPCTSTR pszCallSign, CRepeater*& pResultRepeater)
{
	return m_ToMap.Lookup (pszCallSign, (void*&)pResultRepeater);
}

VOID CRepeaterList::Save (LPCTSTR pszFileName)
{
	CRepeater*		pRepeater;
	POSITION		Pos;
	CString			strLine;
	CStringA		strLineA;
	CFile			File;
	CFileException	Excep;
	LPCSTR			pszHeader	= "Group No,Group Name,Name,Sub Name,Repeater Call Sign,Gateway Call Sign,Frequency,Dup,Offset,Mode,TONE,Repeater Tone,RPT1USE,,,,\r\n";


	do
	{
		if (!File.Open (pszFileName, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, &Excep))
		{	break;
		}

		File.SetLength (0);
		File.Write (pszHeader, lstrlenA (pszHeader));

		Pos = m_List.GetHeadPosition ();
		while (Pos != NULL)
		{
			pRepeater = m_List.GetNext (Pos);
		
			strLine = _T("");
			strLine.AppendFormat (_T("%u"), pRepeater->GetGroup ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetGroupName ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetName ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetSubName ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetCallsign ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetGateway ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetFrequencyStr ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetDup () == 0x12 ? _T("DUP+") : pRepeater->GetDup () == 0x11 ? _T("DUP-") : _T("OFF"));
			strLine.AppendFormat (_T(",%s"), pRepeater->GetOffsetStr ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetDvMode () ? _T("DV") : _T("FM"));
			strLine.AppendFormat (_T(",%s"), pRepeater->GetToneEnable () ? _T("ON") : _T("OFF"));
			strLine.AppendFormat (_T(",%s"), pRepeater->GetRepeaterToneStr ());
			strLine.AppendFormat (_T(",%s"), pRepeater->GetRpt1Use () ? _T("YES") : _T("NO"));
			strLine += _T(",,,,\r\n");

			strLineA = strLine;

			File.Write (strLineA.GetString (), strLineA.GetLength ());
		}
	}
	while (0);


}

VOID CRepeaterList::Load (LPCTSTR pszFileName)
{
	CString			strText;
	CString			strDisplay;
	CString			strLine;
	CString			strCallsign;
	CString			strName;
	CStringArray	Array;
	CStringArray	HeaderArray;
	INT				sTextIndex		= 0;
	INT				eTextIndex;
	ULONG			GroupNo;
	CRepeater*		pRepeater;
	CRepeater*		pGateRepeater;
	CRepeater*		pTmpRepeater;
	

	do
	{
		Clear ();

		if (!FileReadString (pszFileName, &strText))
		{	break;
		}

		eTextIndex = strText.Find (_T("\n"), sTextIndex);
		if (eTextIndex == -1)
		{	break;
		}

		strLine = strText.Mid (sTextIndex, eTextIndex - sTextIndex);
		sTextIndex = eTextIndex + 1;

		//Header Load
		CSV_Split (strLine, &HeaderArray);

		while (1)
		{
			eTextIndex = strText.Find (_T("\n"), sTextIndex);
			if (eTextIndex == -1)
			{	break;
			}

			strLine = strText.Mid (sTextIndex, eTextIndex - sTextIndex);
			sTextIndex = eTextIndex + 1;

			CSV_Split (strLine, &Array);
			
			if (Array.GetCount () < 13)
			{	continue;
			}

			if (HeaderArray.GetAt (9) == _T("Mode"))
			{
			}
			else
			{
				for (INT_PTR i=Array.GetCount () - 1;i>=9;i--)
				{
					Array.SetAtGrow (i + 3, Array.GetAt (i));
				}
				Array.SetAtGrow (11, _T("88.5Hz"));
				Array.SetAtGrow (10, _T("OFF"));
				Array.SetAtGrow (9, _T("DV"));
//0       ,1         ,2   ,3       ,4                 ,5                ,6        ,7  ,8     ,9      ,10      ,11           ,12       ,13      ,14      ,15       ,16               
//Group No,Group Name,Name,Sub Name,Repeater Call Sign,Gateway Call Sign,Frequency,Dup,Offset,Mode   ,TONE    ,Repeater Tone,RPT1USE  ,Position,Latitude,Longitude,UTC Offset
//Group No,Group Name,Name,Sub Name,Repeater Call Sign,Gateway Call Sign,Frequency,Dup,Offset,RPT1USE,Position,Latitude     ,Longitude,UTC Offset

			}


			GroupNo = _ttol (Array.GetAt (0));
			if (GroupNo == 0)
			{	continue;
			}

			strDisplay = Array.GetAt (2);
			if (!Array.GetAt (4).IsEmpty ())
			{
				strDisplay.AppendFormat (_T(" (%s)"), Array.GetAt (4).GetString ());
			}
			pRepeater = new CRepeater;
			pRepeater->Set (&Array);

			m_List.AddTail (pRepeater);
			
			if (::lstrlen (pRepeater->GetCallsign ()) == 0)
			{
				strCallsign.Format (_T("%I64u"), pRepeater->GetFrequency ());
			}
			else
			{
				strCallsign = pRepeater->GetCallsign ();
			}

			if (!m_Map.Lookup (strCallsign, (void*&)pTmpRepeater))
			{
				m_Map.SetAt (strCallsign, pRepeater);
			}

			if (!m_ToMap.Lookup (pRepeater->GetToCallsign (), (void*&)pTmpRepeater))
			{
				m_ToMap.SetAt (pRepeater->GetToCallsign (), pRepeater);
			}



			//Gateway Rename •½–ì430->•½–ì
			if (!pRepeater->GetDvMode ())
			{	continue;
			}

			strCallsign = pRepeater->GetCallsign ();
			if (strCallsign.IsEmpty ())
			{	continue;
			}
			ASSERT (strCallsign.GetLength () == 8);
			if (strCallsign.GetLength () != 8)
			{	continue;
			}

			strCallsign.SetAt(7, 'G');
			
			if (m_Map.Lookup (strCallsign, (void*&)pRepeater))
			{	continue;
			}

			strName = pRepeater->GetName ();
			while ((strName.GetAt (strName.GetLength () - 1) >= '0') && (strName.GetAt (strName.GetLength () - 1) <= '9'))
			{
				strName = strName.Left (strName.GetLength () - 1);
				if (strName.IsEmpty ())
				{	break;
				}
			}

			Array.SetAt (2, strName);
			Array.SetAt (4, strCallsign);
			DBG_MSG((_T("%s %s\n"), strCallsign.GetString (), strName.GetString ()));
			pGateRepeater = new CRepeater;
			pGateRepeater->Set (&Array);
			m_Map.SetAt (pGateRepeater->GetCallsign (), pGateRepeater);
		}
	}
	while (0);
}
