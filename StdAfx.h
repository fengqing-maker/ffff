// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4E6AB207_FFFA_4604_8CE1_F48D9645878E__INCLUDED_)
#define AFX_STDAFX_H__4E6AB207_FFFA_4604_8CE1_F48D9645878E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning( disable:4786)
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include  <math.h>
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <winsock2.h>
#include <Afxmt.h>         // MFC extensionsAfxmt.h


#include "AdoRecordset.h"
#include "sizecbar.h" 
#include "scbarg.h" 



#define	 A4_W			  21000				//A4纸宽度0.01mm
#define	 A4_H			  29700				//A4纸高度0.01mm
#define	 A4_VLINE		  42				//A4纸纵向绘制时行数
#define  A4_HLINE         28                //A4纸横向打印时的行数
#define  A4_HEAD_INTERVAL 2200              //上边缘距离打印处的距离
#define  A4_H_FRAME       2000              //右（左）边缘距离打印处的距离                  
#define  CURVE_DIS_LINE   2                 //波形显示中模拟量通道占用2行
#define  ANALYSE_DIS_LINE 4                 //波形分析中模拟量通道点用6行
#define  DIGITAL_DIS_LINE 1                 //开入开出量点用2行
#define  ROW_HEIGHT       600               //每行的高度为7mm    

#define	 DVI_DEVICE  _T("SID-DVI")


#include "resource.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4E6AB207_FFFA_4604_8CE1_F48D9645878E__INCLUDED_)
