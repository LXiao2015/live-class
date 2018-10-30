#pragma once
#include "av_sdk.h"

using namespace tencent::av;

// VideoRender

class VideoRender : public CWnd
{
	DECLARE_DYNAMIC(VideoRender)

public:
	VideoRender();
	virtual ~VideoRender();
	
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

	void DoRender(VideoFrame* frame); 
 
public:
	void Init(int width, int height, ColorFormat colorFormat);
	void Clear();
  BOOL SnapShot(const char* szBmpName);

public:
  std::string m_identifier;
  VideoSrcType m_videoSrcType;

private:
	ColorFormat m_colorFormat;
	bool m_bRenderHasData;
	bool m_bRenderPaint;
	uint32 m_frameWidth;
	uint32 m_frameHeight;
	uint8 *m_pFrameDataBuf;
	uint8 *m_pBkgDataBuf;
	uint32 m_frameDataBufLen;

	uint8 *m_pRenderDataBuf;
	uint32 m_nRenderDataBufLen;

public:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext);
};


