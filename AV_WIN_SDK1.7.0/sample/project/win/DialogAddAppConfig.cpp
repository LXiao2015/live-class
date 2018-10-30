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
		ShowMessageBox(_T("体验场景不能为空！"));
		return;
	}

	if(accountType.GetLength() == 0)
	{
		ShowMessageBox(_T("账号类型不能为空！"));
		return;
	}

	if(appIdAt3rd.GetLength() == 0)
	{
		ShowMessageBox(_T("appIdAt3rd不能为空！"));
		return;
	}

	if(sdkAppId.GetLength() == 0)
	{
		ShowMessageBox(_T("sdkAppId不能为空！"));
		return;
	}

	if(sceneType.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("体验场景不能含空格！"));
		return;
	}

	if(accountType.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("账号类型不能含空格！"));
		return;
	}
	
	if(appIdAt3rd.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("appIdAt3rd不能含空格！"));
		return;
	}

	if(sdkAppId.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("sdkAppId不能含空格！"));
		return;
	}

	if((accountType != "107") || (sdkAppId != "1104620500"))
	{
		int iRet = ::MessageBoxW(GetSafeHwnd(), 
			_T("你使用的不是默认测试用APPID和ACCTYPE，如果需要指引，请查看该链接:\n\rhttp://bbs.qcloud.com/forum.php?mod=viewthread&tid=8287&extra=page%3D1%26filter%3Dsortid%26sortid%3D6%26sortid%3D6\n\r需要继续添加请点击确定，不需要添加请点击取消."), 
			_T("提示！"), MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL);
		
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
