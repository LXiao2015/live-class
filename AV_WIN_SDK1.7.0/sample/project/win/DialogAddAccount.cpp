// DialogAddAccount.cpp : implementation file�����ļ��Լ��ӵ�
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "DialogAddAccount.h"
#include "afxdialogex.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
// DialogAddAccount dialog

IMPLEMENT_DYNAMIC(DialogAddAccount, CDialogEx)

DialogAddAccount::DialogAddAccount(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogAddAccount::IDD, pParent)
{

}

DialogAddAccount::~DialogAddAccount()
{
	
}

void DialogAddAccount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IDENTIFIER, m_editIdentifier);
	DDX_Control(pDX, IDC_EDIT_USER_SIG, m_editUserSig);
    m_editUserSig.SetWindowTextW(_T("123"));
}


BEGIN_MESSAGE_MAP(DialogAddAccount, CDialogEx)
	ON_BN_CLICKED(IDOK, &DialogAddAccount::OnBnClickedOk)
    END_MESSAGE_MAP()


// DialogAddAccount message handlers


void DialogAddAccount::OnBnClickedOk()
{
	CString identifier = _T(""), userSig = _T("");
	
	m_editIdentifier.GetWindowTextW(identifier);
	m_editUserSig.GetWindowTextW(userSig);
	if(identifier.GetLength() == 0)
	{
		ShowMessageBox(_T("�˺Ų���Ϊ�գ�"));
		return;
	}

	if(userSig.GetLength() == 0)
	{
		ShowMessageBox(_T("���벻��Ϊ�գ�"));
		return;
	}

	if(identifier.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("�˺Ų��ܺ��ո�"));
		return;
	}

	if(userSig.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("���벻�ܺ��ո�"));
		return;
	}
	AccountInfo info;
	info.identifier = identifier;
	info.userSig = userSig;
	ConfigInfoMgr::GetInst()->AddAccount(info);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	ConfigInfoMgr::GetInst()->LoadConfigInfo();
	CDialogEx::OnOK();
}
