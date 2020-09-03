// AdoRecordset.h: interface for the CAdoRecordset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADORECORDSET_H__CE4B38FB_C51F_406A_BF7D_0ECCDFA3E83D__INCLUDED_)
#define AFX_ADORECORDSET_H__CE4B38FB_C51F_406A_BF7D_0ECCDFA3E83D__INCLUDED_
#include "AdoDatabase.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAdoRecordset
{
private:
	void Init();
	DbField field;
public:
	bool IsOpen();
	bool Delete();
	CAdoRecordset(); 
	CAdoRecordset(_RecordsetPtr& lpRecordset);  
	~CAdoRecordset(void);
	void Attach(_RecordsetPtr& lpRecordset); 
	
	void MoveFirst();
	void MoveLast();
	void MoveNext();
	void MovePrev();
	void Move(long rowIndex);
	void AddNew();
	bool Update();
	bool Open(CString sql,CAdoDatabase& database,LockTypeEnum lockType=adLockReadOnly,CursorTypeEnum cursorType=adOpenStatic,
		CommandTypeEnum cmdType=adCmdText);
	void Requery() { m_lpRecordset->Requery(adOptionUnspecified);}
	void Close()   { m_lpRecordset->Close(); }
	long GetRecordCount() { return m_lpRecordset->RecordCount; } 
	bool IsEOF() { return (bool)(m_lpRecordset->adoEOF); }
	bool IsBOF() { return (bool)(m_lpRecordset->BOF); } 
	
	//ÔËËã·ûÖØÔØ------------------------------
	DbField& operator[](const wchar_t * fieldName)
	{
		field.Attach(m_lpRecordset->GetFields()->GetItem(_variant_t(fieldName)));
		return field;
	}
	
	// --- Members ----------------
	_RecordsetPtr m_lpRecordset;
	//----------------------------

}; 


#endif // !defined(AFX_ADORECORDSET_H__CE4B38FB_C51F_406A_BF7D_0ECCDFA3E83D__INCLUDED_)
