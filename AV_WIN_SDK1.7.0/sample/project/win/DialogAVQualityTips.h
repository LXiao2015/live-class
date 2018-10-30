#pragma once
#include "afxwin.h"
#include "VideoRender.h"

// DialogAVQualityTips dialog

class DialogAVQualityTips : public CDialogEx
{
	DECLARE_DYNAMIC(DialogAVQualityTips)

public:
	DialogAVQualityTips(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogAVQualityTips();

// Dialog Data
	enum { IDD = IDD_DIALOG_AV_QUALITY_TIPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	void UpdateTips(CString tips);

private:
	CEdit m_editAVQualityTips;
};
