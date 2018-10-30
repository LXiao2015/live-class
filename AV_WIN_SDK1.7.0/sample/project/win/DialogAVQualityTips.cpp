// DialogAVQualityTips.cpp : implementation file
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "DialogAVQualityTips.h"
#include "afxdialogex.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
#include "DialogQAVSDKDemo.h"
// DialogAVQualityTips dialog

IMPLEMENT_DYNAMIC(DialogAVQualityTips, CDialogEx)

DialogAVQualityTips::DialogAVQualityTips(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogAVQualityTips::IDD, pParent)
{
	
}

DialogAVQualityTips::~DialogAVQualityTips()
{
	
}

void DialogAVQualityTips::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_QUALITY_TIPS, m_editAVQualityTips);
}

void DialogAVQualityTips::OnClose()
{	
	DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)this->GetParent();
	if(pMainDlg)pMainDlg->CloseAVQualityTips();	
}

void DialogAVQualityTips::UpdateTips(CString tips)
{	
	m_editAVQualityTips.SetWindowTextW(tips);
}

BEGIN_MESSAGE_MAP(DialogAVQualityTips, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// DialogAVQualityTips message handlers
