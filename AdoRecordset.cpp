// AdoRecordset.cpp: implementation of the CAdoRecordset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdoRecordset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-----------------------------------------------------
// CAdoRecordset  实现 
//----------------------------------------------------- 

void CAdoRecordset::Init()
{
	m_lpRecordset = NULL;
	m_lpRecordset.CreateInstance("ADODB.Recordset");  
} 

CAdoRecordset::CAdoRecordset()
{
	Init();
} 

CAdoRecordset::CAdoRecordset(_RecordsetPtr& lpRecordset)
{
	Init();
	m_lpRecordset = lpRecordset;
} 


CAdoRecordset::~CAdoRecordset()
{
	if( m_lpRecordset != NULL)
	{
		m_lpRecordset.Release(); 
	}
} 

void CAdoRecordset::Attach(_RecordsetPtr& lpRecordset)
{
	if (m_lpRecordset != NULL)
		m_lpRecordset.Release(); 
	m_lpRecordset = lpRecordset;
} 

void CAdoRecordset::MoveFirst()
{
	m_lpRecordset->MoveFirst();
} 

void CAdoRecordset::MoveLast()
{
	m_lpRecordset->MoveLast();
} 


void CAdoRecordset::MoveNext()
{
	m_lpRecordset->MoveNext();
} 


void CAdoRecordset::MovePrev()
{
	m_lpRecordset->MovePrevious();
} 

void CAdoRecordset::Move(long rowIndex)
{
	m_lpRecordset->Move(rowIndex);
} 

void CAdoRecordset::AddNew()
{
	m_lpRecordset->AddNew();
} 

bool CAdoRecordset::Update()
{
	return m_lpRecordset->Update() == 0;
} 

bool CAdoRecordset::Open(CString sql,CAdoDatabase& database,LockTypeEnum lockType/*=adLockReadOnly*/,
						 CursorTypeEnum cursorType/*=adOpenStatic*/,
						 CommandTypeEnum cmdType/*=adCmdText*/)
{
	wchar_t * sqlText = sql.GetBuffer(0); 
	m_lpRecordset->Open(sqlText,(IDispatch*)database.m_lpConnection,cursorType,lockType,NULL/*cmdType*/);
	//m_lpRecordset->Open(sqlText, (IDispatch*)conn.m_lpConnection,adOpenStatic, adLockOptimistic, NULL);
	sql.ReleaseBuffer(); 
	return true;
}

//----------------------------------------------------
//函数实现
//---------------------------------------------------- 

CString vartostr(const _variant_t &var)
{
	CString strValue;
	switch (var.vt)
	{
	case VT_BSTR://字符串
	case VT_LPSTR://字符串
	case VT_LPWSTR://字符串
		strValue = (LPCTSTR)(_bstr_t)var;
		break;
	case VT_I1:
	case VT_UI1:
		strValue.Format(_T("%d"), var.bVal);
		break;
	case VT_I2://短整型
		strValue.Format(_T("%d"), var.iVal);
		break;
	case VT_UI2://无符号短整型
		strValue.Format(_T("%d"), var.uiVal);
		break;
	case VT_INT://整型
		strValue.Format(_T("%d"), var.intVal);
		break;
	case VT_I4: //整型
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_I8: //长整型
		strValue.Format(_T("%d"), var.lVal);
		break;
	case VT_UINT://无符号整型
		strValue.Format(_T("%d"), var.uintVal);
		break;
	case VT_UI4: //无符号整型
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_UI8: //无符号长整型
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_VOID:
		strValue.Format(_T("%8x"), var.byref);
		break;
	case VT_R4://浮点型
		strValue.Format(_T("%.4f"), var.fltVal);
		break;
	case VT_R8://双精度型
		strValue.Format(_T("%.8f"), var.dblVal);
		break;
	case VT_DECIMAL: //小数
		strValue.Format(_T("%.8f"), (double)var);
		break;
	case VT_CY:
		{
			//货币型保留四位小数
			strValue.Format(_T("%.4f"), (float)((float)var.cyVal.int64 / (float)10000));   
		}
		break;
	case VT_BLOB:
	case VT_BLOB_OBJECT:
	case 0x2011:
		strValue = _T("[BLOB]");
		break;
	case VT_BOOL://布尔型
		strValue = var.boolVal ? _T("TRUE") : _T("FALSE");
		break;
	case VT_DATE: //日期型
		{
			DATE dt = var.date;
			COleDateTime da = COleDateTime(dt); 
			strValue = da.Format(_T("%Y-%m-%d %H:%M:%S"));     
		}
		break;
	case VT_NULL://NULL值
		strValue = _T("");
		break;
	case VT_EMPTY://空
		strValue =_T("");
		break;
	case VT_UNKNOWN://未知类型
	default:
		strValue = _T("UN_KNOW");
		break;
	}
	return strValue;
}

bool CAdoRecordset::Delete()
{
	return m_lpRecordset->Delete(adAffectCurrent)==0;
}

bool CAdoRecordset::IsOpen()
{
	if(m_lpRecordset->State==adStateClosed)
	{
		return false;
	}
	else
	{
		return true;
	}
}
