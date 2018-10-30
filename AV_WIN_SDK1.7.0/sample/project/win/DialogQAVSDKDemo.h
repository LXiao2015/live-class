
// DialogQAVSDKDemo.h : header file
//

#pragma once
#include "afxwin.h"
#include "resource.h"	
#include "VideoRender.h"
#include "ListBoxEndpoint.h"
#include "SdkWrapper.h"
#include "CustomWinMsg.h"
#include "tim.h"
#include "tim_comm.h"
#include "tim_int.h"
#include "Util.h"
#include "Dialog1v1VideoRender.h"
#include "DialogAVQualityTips.h"

using namespace imcore;
using namespace tencent::av;
class LoginCallBack : public TIMCallBack 
{
	virtual void OnSuccess();
	virtual void OnError(int retCode, const std::string &desc);
};

class LogoutCallBack : public TIMCallBack 
{
	virtual void OnSuccess();
	virtual void OnError(int retCode, const std::string &desc);
};

class PushVideoStartCallBack:public TIMValueCallBack<TIMStreamRsp&>
{
	virtual void OnSuccess(TIMStreamRsp& value) override;
	virtual void OnError(int code, const std::string &desc) override;
};

class PushVideoStopCallBack: public TIMCallBack
{
	virtual void OnSuccess() override;
	virtual void OnError(int code, const std::string &desc) override;
};

class RecordVideoStartCallBack:public TIMCallBack
{
	virtual void OnSuccess() override;
	virtual void OnError(int code, const std::string &desc) override;
};

class RecordVideoStopCallBack:public TIMValueCallBack<std::list<std::string>&>
{
private: typedef std::list<std::string>& CallBackDataType;
	virtual void OnSuccess(CallBackDataType data) override;
	virtual void OnError(int code, const std::string &desc) override;
};

// DialogQAVSDKDemo dialog
class DialogQAVSDKDemo : public CDialogEx
{
	friend class SdkWrapper;
// Construction
public:
	DialogQAVSDKDemo(CWnd* pParent = NULL);	// standard constructor
	~DialogQAVSDKDemo();
// Dialog Data
	enum { IDD = IDD_DIALOG_DEMO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// msg or command
  //系统部分
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnGetDefId(WPARAM wp, LPARAM lp);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);  //消息响应函数，响应WM_CTLCOLOR消息，被父窗口发送已改变对话框或对话框上面控件的颜色
	afx_msg void OnTimer(UINT_PTR eventId);
	afx_msg void TrackMenu(CWnd*pWnd);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


  //用户自定义部分	
	afx_msg LONG OnLogin(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnLogout(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStartContext(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStopContext(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnEnterRoom(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnExitRoom(WPARAM wParam, LPARAM lParam); 	
	afx_msg LONG OnRequestViewList(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnCancelAllView(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnCameraOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnMicOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnPlayerOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnScreenShareSendOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnAccompanyOperation(WPARAM wParam, LPARAM lParam); 	
	afx_msg LONG OnEnpointsUpdateInfo(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnEndpointMenuItem(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnAudioDeviceDetect(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnLButtonDBClick(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStartDeviceTest(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStopDeviceTest(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnDeviceTestCameraOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnDeviceTestMicOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnDeviceTestPlayerOperation(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnPushVideoStart(WPARAM wParam, LPARAM lParam); //声明的是一个消息响应函数
	afx_msg LONG OnPushVideoStop(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnRecordVideoStart(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnRecordVideoStop(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedBtnOpenUserManual();
	afx_msg void OnBnClickedBtnEnableTestEnv();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLogout();
	afx_msg void OnBnClickedBtnEnterRoom();
	afx_msg void OnBnClickedBtnExitRoom();
	afx_msg void OnBnClickedBtnOpenCamera();
	afx_msg void OnBnClickedBtnCloseCamera();
	afx_msg void OnBnClickedBtnOpenMic();
	afx_msg void OnBnClickedBtnCloseMic();
	afx_msg void OnBnClickedBtnOpenPlayer();
	afx_msg void OnBnClickedBtnClosePlayer();
	afx_msg void OnBnClickedBtnSnapShot();	
	afx_msg void OnBnClickedBtnOpenAVQualityTips();  
	afx_msg void OnBnClickedBtnCloseAVQualityTips();  
	afx_msg void OnBnClickedBtnstartaccompany();
	afx_msg void OnBnClickedBtnstopaccompany();
	afx_msg void OnBnClickedBtnStartAccompany();  
	afx_msg void OnBnClickedBtnStopAccompany();
	afx_msg void OnBnClickedChkEnableAec();
	afx_msg void OnBnClickedChkEnableNs();
	afx_msg void OnBnClickedChkEnableBoost();
  
	afx_msg void OnBnClickedBtnAddAccount();
	afx_msg void OnBnClickedBtnDeleteAccount();
	afx_msg void OnBnClickedBtnAddChannel();
	afx_msg void OnBnClickedBtnDeleteChannel();
	afx_msg void OnBnClickedBtnAddApp();
	afx_msg void OnBnClickedBtnDeleteApp(); 

	afx_msg void OnBnClickedBtnCheckYuvOutput();
	afx_msg void OnCbnSelchangeComboSceneTypeList();  
	afx_msg void OnEnChangeEditRoomId();  

	afx_msg void OnBnClickedBtnAudioDataMic();
	afx_msg void OnBnClickedBtnAudioDataMixToSend();
	afx_msg void OnBnClickedBtnAudioDataSend();
	afx_msg void OnBnClickedBtnAudioDataNetStream();
	afx_msg void OnBnClickedBtnAudioDataMixToPlay();
	afx_msg void OnBnClickedBtnAudioDataPlay();

	afx_msg void OnCbnSelchangeComboAudioDataType();
	afx_msg void OnCbnSelchangeComboAudioDataParam();
	afx_msg void OnChangeAudioDataVolume();

	afx_msg void OnBnClickedBtnStartDeviceTest();
	afx_msg void OnBnClickedBtnStopDeviceTest();
	afx_msg void OnBnClickedBtnDeviceTestOpenCamera();
	afx_msg void OnBnClickedBtnDeviceTestCloseCamera();
	afx_msg void OnBnClickedBtnDeviceTestOpenMic();
	afx_msg void OnBnClickedBtnDeviceTestCloseMic();
	afx_msg void OnBnClickedBtnDeviceTestOpenPlayer();
	afx_msg void OnBnClickedBtnDeviceTestClosePlayer();
  
	DECLARE_MESSAGE_MAP()

	void OnVideoPreTreatment(VideoFrame* pFrame);

private:
	void UpadateVideoRenderInfo();
	void UpdateConfigInfo();	
	void UpdateEndpointList();
	void ShowRetCode(std::string tipsStr, int retCode);
	void ShowOperationTips(CString tips, int timeLen);
	void ClearOperationTips();
	void ChangeAuthComplete(int retCode);
	void PrepareEncParam();
	void PrepareYUVData();
	void UpdateAudioDataState();
	void UpdateVideoPushBtnsState();

private:
	HICON m_hIcon;

	CComboBox m_comboAccountList;
	CComboBox m_comboChannelList;  //自己加的
	CComboBox m_comboSceneTypeList;
	CComboBox m_comboCameraList;
	CComboBox m_comboMicList;
	CComboBox m_comboPlayerList;
	CComboBox m_comboVideoInputType;
	CComboBox m_comboDeviceTestCameraList;
	CComboBox m_comboDeviceTestMicList;
	CComboBox m_comboDeviceTestPlayerList;
	CComboBox m_comboScreenShareModeList;

	ListBoxEndpoint m_listEndpointList;	  

	CButton m_btnOpenUserManual;
	CButton m_btnEnableTestEnv;
	CButton m_btnSnapShot;
	CButton m_btnLogin;
	CButton m_btnLogout;
	CButton m_btnEnterRoom;
	CButton m_btnExitRoom;
	CButton m_btnAddAccount;
	CButton m_btnAddChannel;  //自己加的
	CButton m_btnDeleteAccount;
	CButton m_btnDeleteChannel;  //自己加的
	CButton m_btnOpenCamera;
	CButton m_btnCloseCamera;
	CButton m_btnOpenMic;
	CButton m_btnCloseMic;
	CButton m_btnOpenPlayer;
	CButton m_btnClosePlayer;
	CButton m_btnOpenScreenShareSend;
	CButton m_btnCloseScreenShareSend;
	CButton m_chkAudioEnableAEC;
	CButton m_chkAudioEnableNS;
	CButton m_chkAudioEnableBoost;
	CButton m_btnOpenAVQualityTips;
	CButton m_btnCloseAVQualityTips;
	CButton m_btnStartDeviceTest;
	CButton m_btnStopDeviceTest;
	CButton m_btnDeviceTestOpenCamera;
	CButton m_btnDeviceTestCloseCamera;
	CButton m_btnDeviceTestOpenMic;
	CButton m_btnDeviceTestCloseMic;
	CButton m_btnDeviceTestOpenPlayer;
	CButton m_btnDeviceTestClosePlayer;
	CButton m_btnOpenExternalCapture;
	CButton m_btnCloseExternalCapture;


	CEdit m_editAccountType;
	CEdit m_editAppIdAt3rd;
	CEdit m_editSdkAppId;
	CEdit m_editRoomId;
	CEdit m_editRoomRole;
	CEdit m_editChannelDescription;  //自己加的
	
	CEdit m_editPlayerPath;
	CEdit m_editMediaPath;
	CEdit m_editYuvInputPath;
	CEdit m_editYuvInputWidth;
	CEdit m_editYuvInputHeight;

	CEdit m_editScreenShareCaptureLeft;
	CEdit m_editScreenShareCaptureTop;
	CEdit m_editScreenShareCaptureRight;
	CEdit m_editScreenShareCaptureBottom;
	CEdit m_editScreenShareCaptureFps;
	
	//CStatic m_staticSDKVersion;
	CStatic m_staticOperationTips;
	CStatic m_staticMicCurVolume;	
	CStatic m_staticPlayerCurVolume;		
	CStatic m_staticAcompanyCurVolume;
	
	CSliderCtrl m_sliderAudioMicVolume;
	CSliderCtrl m_sliderAudioPlayerVolume;
	CSliderCtrl m_sliderAccompanyVolume;
	CSliderCtrl m_sliderDeviceTestAudioMicVolume;
	CSliderCtrl m_sliderDeviceTestAudioPlayerVolume;

	CEdit m_editProtoAuthBuffer;

	CButton m_btnProtoAuth;
	CButton m_btnCheckYuvOutput;

	CButton m_btnCheckHasVideoSendAuth;
	CButton m_btnCheckHasAudioSendAuth;
	CButton m_btnCheckHasVideoRevAuth;
	CButton m_btnCheckHasAudioRevAuth;
	CButton m_chkIsAutoCreateSDKRoom;

	CButton m_btnCheckAudioDataMic;
	CButton m_btnCheckAudioDataMixToSend;
	CButton m_btnCheckAudioDataSend;
	CButton m_btnCheckAudioDataNetStream;
	CButton m_btnCheckAudioDataMixToPlay;
	CButton m_btnCheckAudioDataPlay;

	CButton m_btnPushVideoReq;
	CButton m_btnRecordVideoReq;

	CSliderCtrl m_sliderAudioDataVolume;

	CComboBox m_comboAudioDataType;
	CComboBox m_comboAudioDataSampleRate;
	CComboBox m_comboAudioDataChannelNum;

	CComboBox m_comboPushVideoDataType;
	CComboBox m_comboPushVideoEncodeType;
	CComboBox m_comboRecordVideoDataType;

	CEdit m_editMixToSendPath;
	CEdit m_editMixToPlayPath;

	SdkWrapper m_sdkWrapper;	
	AVContext::Config m_config;
	std::string m_userSig;

	VideoRender m_viewLocalVideoRender;
	VideoRender m_viewBigVideoRender;
	//VideoRender m_viewRemoteVideoSmallRender1;
	//VideoRender m_viewRemoteVideoSmallRender2;
	//VideoRender m_viewRemoteVideoSmallRender3;
	//VideoRender m_viewRemoteVideoSmallRender4;
	VideoRender m_viewScreenShareRender;
  
	bool m_isExitDemo;
	bool m_isTestEnv;
	bool m_isLogin;
	bool m_isEnterRoom;	
	bool m_isEnableMic;
	bool m_isEnablePlayer;
	bool m_isEnableScreenShareSend;
	bool m_isEnableCamera;
	bool m_isStartDeviceTest;	
	bool m_isDeviceTestEnableMic;
	bool m_isDeviceTestEnablePlayer;
	bool m_isDeviceTestEnableCamera;
	bool m_isVideoPushing;
	bool m_isVideoRecording;

	LoginCallBack m_loginCallBack;
	LogoutCallBack m_logoutCallBack;
	PushVideoStartCallBack m_pushVideoStartCallBack;
	PushVideoStopCallBack m_pushVideoStopCallBack;
	RecordVideoStartCallBack m_recordVideoStartCallBack;
	RecordVideoStopCallBack m_recordVideoStopCallBack;

	int m_mainDialogLeft;
	int m_mainDialogTop;
	int m_mainDialogWidth;
	int m_mainDialogHeight;

	CString m_curSceneType;

	//一些比较耗时的操作提示
	CString m_operationTipsLogining;
	CString m_operationTipsLogouting;
	CString m_operationTipsEnterRooming;
	CString m_operationTipsExitRooming;
	CString m_operationTipsOpenCameraing;
	CString m_operationTipsCloseCameraing;
	CString m_operationTipsReqeustViewing;
	CString m_operationTipsCancelViewing;

	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_micList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_playerList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_cameraList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_deviceTestMicList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_deviceTestPlayerList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_deviceTestCameraList;

	bool m_bEnablePreTreat;
	uint8* m_pYUVData;
	uint32 m_nYUVWidth;
	uint32 m_nYUVHeight;

	bool m_bEnableExCap;
	uint8* m_pYUVCapData;
	uint32 m_nYUVCapWidth;
	uint32 m_nYUVCapHeight;

	uint64 m_uPushVideoChID;

	Dialog1v1VideoRender* m_p1v1VideoRenderDlg;
	DialogAVQualityTips* m_pAVQualityTipsDlg;

public:
	afx_msg void OnCbnSelchangeComboCameraList();
	afx_msg void OnCbnSelchangeComboMicList();
	afx_msg void OnCbnSelchangeComboPlayerList();
	afx_msg void OnBnClickedBtnProaut();
	afx_msg void OnBnClickedHighauth();
	afx_msg void OnBnClickedLowauth();
	afx_msg void OnBnClickedBtnOpenExteralCapture();
	afx_msg void OnBnClickedBtnCloseExteralCapture();
	afx_msg void OnBnClickedEnablepretreat();
	afx_msg void OnBnClickedDisablepretreat();
	afx_msg void OnCbnSelchangeComboDeviceTestCameraList();
	afx_msg void OnCbnSelchangeComboDeviceTestMicList();
	afx_msg void OnCbnSelchangeComboDeviceTestPlayerList();

	void SaveRemoteYuvFrame(VideoFrame *pFrameData);
	void Open1v1View();
	void Close1v1View();
	void OpenAVQualityTips();
	void CloseAVQualityTips();
private:
	FILE* m_pYuvInputFile;
	FILE* m_pYuvOutputFile;
	int m_rectYuvOutputWidth;
	int m_rectYuvOutputHeight;
public:
	afx_msg void OnBnClickedBtnOpenScreenShareSend();
	afx_msg void OnBnClickedBtnCloseScreenShareSend();
	afx_msg void OnCbnSelchangeComboScreenShareModeList();
	afx_msg void OnBnClickedBtnOpen1v1View();
	afx_msg void OnBnClickedBtnClose1v1View();

	afx_msg void OnClickedBtnPushVideoReq();
	afx_msg void OnClickedBtnRecordVideoReq();
	CEdit m_editProtoAuthBit;
	CEdit para_file_path;
	afx_msg void OnBnClickedButtonSearchPara();
	afx_msg void OnStnClickedViewScreenShare();
	afx_msg void OnBnClickedCheckIsAutoCreateSdkRoom();
	afx_msg void OnStnClickedStaticRoomId();
	afx_msg void OnStnClickedStaticCamera();
	afx_msg void OnBnClickedButton2();
};
