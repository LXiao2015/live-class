#pragma once
#include "afxwin.h"


// DialogAddAccount dialog

class DialogAddAccount : public CDialogEx
{
	DECLARE_DYNAMIC(DialogAddAccount)

public:
	DialogAddAccount(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogAddAccount();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editIdentifier;
	CEdit m_editUserSig;
	afx_msg void OnBnClickedOk();
};
