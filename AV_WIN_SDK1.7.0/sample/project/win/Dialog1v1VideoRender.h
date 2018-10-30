#pragma once
#include "afxwin.h"
#include "VideoRender.h"

// Dialog1v1VideoRender dialog

class Dialog1v1VideoRender : public CDialogEx
{
	DECLARE_DYNAMIC(Dialog1v1VideoRender)

public:
	Dialog1v1VideoRender(CWnd* pParent = NULL);   // standard constructor
	virtual ~Dialog1v1VideoRender();

// Dialog Data
	enum { IDD = IDD_DIALOG_1V1_VIDEO_RENDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
public:
	void UpdateWindowPos(uint32 imageWidth, uint32 imageHeight);

	VideoRender m_view1v1VideoRender;

private:
	uint32 m_imageWidth;
	uint32 m_imageHeight;
};
