// Dialog1v1VideoRender.cpp : implementation file
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "Dialog1v1VideoRender.h"
#include "afxdialogex.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
#include "DialogQAVSDKDemo.h"
// Dialog1v1VideoRender dialog

IMPLEMENT_DYNAMIC(Dialog1v1VideoRender, CDialogEx)

Dialog1v1VideoRender::Dialog1v1VideoRender(CWnd* pParent /*=NULL*/)
	: CDialogEx(Dialog1v1VideoRender::IDD, pParent)
{
	//m_view1v1VideoRender.Init(VIDEO_RENDER_SMALL_VIEW_WIDTH, VIDEO_RENDER_SMALL_VIEW_HEIGHT, COLOR_FORMAT_RGB24);
	//m_view1v1VideoRender.m_videoSrcType = VIDEO_SRC_TYPE_CAMERA;
	m_imageWidth = 0;
	m_imageHeight = 0;
}

Dialog1v1VideoRender::~Dialog1v1VideoRender()
{
	
}

void Dialog1v1VideoRender::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_1V1_VIDEO_RENDER, m_view1v1VideoRender);
}

void Dialog1v1VideoRender::OnClose()
{
	m_view1v1VideoRender.Clear();
	DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)this->GetParent();
	if(pMainDlg)pMainDlg->Close1v1View();	
}

void Dialog1v1VideoRender::UpdateWindowPos(uint32 imageWidth, uint32 imageHeight)
{	
	if(m_imageWidth != imageWidth || m_imageHeight != imageHeight)
	{
		m_imageWidth = imageWidth;
		m_imageHeight = imageHeight;
		SetWindowPos(GetDlgItem(IDD_DIALOG_1V1_VIDEO_RENDER), 0, 0, m_imageWidth, m_imageHeight, SWP_NOZORDER);	
	
		CWnd *pWndVideoRenderView;  
		pWndVideoRenderView = GetDlgItem(IDC_1V1_VIDEO_RENDER); 		
		pWndVideoRenderView->SetWindowPos(NULL, 0, 0, m_imageWidth, m_imageHeight, SWP_NOZORDER); 
	}		
}

BEGIN_MESSAGE_MAP(Dialog1v1VideoRender, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Dialog1v1VideoRender message handlers
