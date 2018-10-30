#pragma once  //整个文件自己加的
#include "afxwin.h"
#include "DialogQAVSDKDemo.h"


// DialogAddChannel dialog

class DialogAddChannel : public CDialogEx
{
	DECLARE_DYNAMIC(DialogAddChannel)

public:
	DialogAddChannel(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogAddChannel();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editChannelName;
	CEdit m_editChannelDescription;
	afx_msg void OnBnClickedOk();
};