// DialogAddChannel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DialogAddChannel.h"
#include "afxdialogex.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
#include "QAVSDKApp.h"
// DialogAddChannel �Ի���

IMPLEMENT_DYNAMIC(DialogAddChannel, CDialogEx)

DialogAddChannel::DialogAddChannel(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogAddChannel::IDD, pParent)
{

}

DialogAddChannel::~DialogAddChannel()
{
}

void DialogAddChannel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHANNEL_NAME, m_editChannelName);
	DDX_Control(pDX, IDC_EDIT_CHANNEL_DESCRIPTION, m_editChannelDescription);
	m_editChannelDescription.SetWindowTextW(_T("��ʦ����--���ΰ༶��"));
}


BEGIN_MESSAGE_MAP(DialogAddChannel, CDialogEx)
	ON_BN_CLICKED(IDOK, &DialogAddChannel::OnBnClickedOk)
END_MESSAGE_MAP()


// DialogAddChannel ��Ϣ�������

void DialogAddChannel::OnBnClickedOk()
{
	CString channelName = _T(""), channelDescription = _T("");
	
	m_editChannelName.GetWindowTextW(channelName);
	m_editChannelDescription.GetWindowTextW(channelDescription);
	if(channelName.GetLength() == 0)
	{
		ShowMessageBox(_T("Ƶ�����Ʋ���Ϊ�գ�"));
		return;
	}

	if(channelDescription.GetLength() == 0)
	{
		ShowMessageBox(_T("Ƶ����������Ϊ�գ�"));
		return;
	}

	if(channelName.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("Ƶ�����Ʋ��ܺ��ո�"));
		return;
	}

	if(channelDescription.Find(_T(" ")) > 0)
	{
		ShowMessageBox(_T("Ƶ���������ܺ��ո�"));
		return;
	}
	ChannelInfo info;
	info.channelName = channelName;
	info.channelDescription = channelDescription;
	ConfigInfoMgr::GetInst()->AddChannel(info);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	ConfigInfoMgr::GetInst()->LoadConfigInfo();
	CDialogEx::OnOK();
}
