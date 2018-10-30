#pragma once
#include "afxwin.h"


// DialogAddAppConfig dialog

class DialogAddAppConfig : public CDialogEx
{
	DECLARE_DYNAMIC(DialogAddAppConfig)

public:
	DialogAddAppConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogAddAppConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_APP_CONFIG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editSceneType;
	CEdit m_editAccountType;
	CEdit m_editAppIdAt3rd;
	CEdit m_editSdkAppId;
	afx_msg void OnBnClickedOk();
};
