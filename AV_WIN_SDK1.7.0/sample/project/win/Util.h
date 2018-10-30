#pragma once
#include <string>
#include "basictypes.h"
#define ShowMessageBox(msg) ::MessageBoxW(GetSafeHwnd(), msg, _T("ÌáÊ¾£¡"), MB_OK | MB_ICONWARNING | MB_APPLMODAL)

CString GetExePath();

CString GetResPath();

std::string StrWToStrA(CStringW w);

CStringW StrAToStrW(CStringA w);

void SetMainHWnd(HWND hWnd);
HWND GetMainHWnd();

#define SAFEDELETE(a) if (a){delete a; a = NULL;}

void LoadBmpData(std::string strFileName, uint8** ppData, uint32 & nWidth, uint32 & nHeight);
void AddImg_I420(uint8* psrc, int32 width, int32 height, uint8* pmark, int32 cx, int32 cy);
int ConvertRGB2YUV(int32 w,int32 h,uint8 *bmp,uint8 *yuv);

void _CopyBits2Tex_None_0(uint8* pDst, uint8* pSrc, UINT uLen, const SIZE & size);
void _CopyBits2Tex_None_270(uint8* pDst, uint8* pSrc, UINT uLen, const SIZE & size);
void _CopyBits2Tex_None_180(uint8* pDst, uint8* pSrc, UINT uLen, const SIZE & size);
void _CopyBits2Tex_None_90(uint8* pDst, uint8* pSrc, UINT uLen, const SIZE & size);
bool _ResizeWithMendBlack(uint8* pDst, uint8* pSrc, UINT uDstLen, UINT uSrcLen, const SIZE & dstSize, const SIZE & srcSize, UINT bpp);

class SimpleLock
{
public:
	SimpleLock(){ ::InitializeCriticalSection(&cs_); }
	~SimpleLock(){ ::DeleteCriticalSection(&cs_); }

	void Lock(){ ::EnterCriticalSection(&cs_); }
	void Unlock(){ ::LeaveCriticalSection(&cs_); }

private:
	CRITICAL_SECTION cs_;
};

class SimpleAutoLock
{
public:
	SimpleAutoLock(SimpleLock* lock):lock_(lock){ lock_->Lock(); }
	~SimpleAutoLock(){ lock_->Unlock(); }

private:
	SimpleLock* lock_;
};