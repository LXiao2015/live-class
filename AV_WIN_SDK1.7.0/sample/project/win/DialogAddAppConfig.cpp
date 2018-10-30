// DialogAddAppConfig.cpp : implementation file
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "DialogAddAppConfig.h"
#include "afxdialogex.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
// DialogAddAppConfig dialog

IMPLEMENT_DYNAMIC(DialogAddAppConfig, CDialogEx)

DialogAddAppConfig::DialogAddAppConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogAddAppConfig::IDD, pParent)
{

}

DialogAddAppConfig::~DialogAddAppConfig()
{
	
}

void DialogAddAppConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SCENE_TYPE, m_editSceneType);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_TYPE, m_editAccountType);
	DDX_Control(pDX, IDC_EDIT_APP_ID_AT_3RD, m_editAppIdAt3rd);
	DDX_Control(pDX, IDC_EDIT_SDK_APP_ID, m_editSdkAppId);
}


BEGIN_MESSAGE_MAP(DialogAddAppConfig, CDialogEx)
	ON_BN_CLICKED(IDOK, &DialogAddAppConfig::OnBnClickedOk)
END_MESSAGE_MAP()


// DialogAddAppConfig message handlers


void DialogAddAppConfig::OnBnClickedOk()
{
	CString sceneType = _T(""), accountType = _T(""), appIdAt3rd = _T(""), sdkAppId = _T("");
	m_editSceneType.GetWindowTextW(sceneType);
	m_editAccountType.GetWindowTextW(accountType);
	m_editAppIdAt3rd.GetWindowTextW(appIdAt3rd);
	m_editSdkAppId.GetWindowTextW(sdkAppId);

	if(sceneType.GetLength() == 0)
	{
		ShowMessageBox(_T("���鳡������Ϊ�գ�"));
		return;
	}

	if(accountType.GetLength() == 0)
	{
		ShowMessageBox(_T("�˺����Ͳ���Ϊ�գ�"));
		return;
	}

	if(appIdAt3rd.GetLength() == 0)
	{
		ShowMessageBox(_T("appIdAt3rd����Ϊ�գ�"));
		return;
	}

	if(sdkAppId.GetLength() == 0)
	{
		ShowMessageBox(_T("sdkAppId����Ϊ�գ�"));
		return;
	}

	if(sceneType.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("���鳡�����ܺ��ո�"));
		return;
	}

	if(accountType.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("�˺����Ͳ��ܺ��ո�"));
		return;
	}
	
	if(appIdAt3rd.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("appIdAt3rd���ܺ��ո�"));
		return;
	}

	if(sdkAppId.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("sdkAppId���ܺ��ո�"));
		return;
	}

	if((accountType != "107") || (sdkAppId != "1104620500"))
	{
		int iRet = ::MessageBoxW(GetSafeHwnd(), 
			_T("��ʹ�õĲ���Ĭ�ϲ�����APPID��ACCTYPE�������Ҫָ������鿴������:\n\rhttp://bbs.qcloud.com/forum.php?mod=viewthread&tid=8287&extra=page%3D1%26filter%3Dsortid%26sortid%3D6%26sortid%3D6\n\r��Ҫ�����������ȷ��������Ҫ�������ȡ��."), 
			_T("��ʾ��"), MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL);
		
		if(iRet == IDCANCEL)
		{
			return;
		}
	}

	AppInfo info;
	info.sceneType = sceneType;
	info.accountType = accountType;
	info.appIdAt3rd = appIdAt3rd;
	info.sdkAppId = sdkAppId;
	ConfigInfoMgr::GetInst()->AddApp(info);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	ConfigInfoMgr::GetInst()->LoadConfigInfo();
	CDialogEx::OnOK();
}
