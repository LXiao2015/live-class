
// DialogQAVSDKDemo.cpp : implementation file
//

#include "stdafx.h"
#include "QAVSDKApp.h"
#include "DialogQAVSDKDemo.h"
#include "afxdialogex.h"
#include "CustomWinMsg.h"
#include "DialogAddAccount.h"
#include "DialogAddChannel.h"
#include "DialogAddAppConfig.h"
#include "Dialog1v1VideoRender.h"
#include "DialogAVQualityTips.h"
#include "ConfigInfoMgr.h"
#include "Util.h"
#include "av_common.h"

//crash上报
#include "crash_report.h"

#define ENABLE_UI_OPERATION_SAFETY

//实时通信场景体验版
#define SCENE_IM_EXPERIENCE _T("实时通信场景体验版")
//主播场景体验版
#define SCENE_ANCHOR_EXPERIENCE _T("主播场景体验版")

//默认场景：实时通信场景体验版
#define DEFAULT_SCENE SCENE_IM_EXPERIENCE

//默认场景：主播场景体验版
//#define DEFAULT_SCENE SCENE_ANCHOR_EXPERIENCE


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ENABLE_QUALITY_TIPS 1
#define TIMER_SHOW_OPERRATION_TIPS 2
#define TIMER_EXTERNAL_CAPTURE 3
#define SHORT_TIME (1000)
#define LONG_TIME (3000)

struct ErrorInfo 
{
	int retCode;
	CString desc;
};

struct AudioDataTypes
{
	int value;
	const TCHAR* discription;
};

struct AudioDataSampleRate
{
	int value;
	const TCHAR* discription;
};

struct AudioDataChannelNumState
{
	int value;
	const TCHAR* discription;
};

struct AudioDataEnableState
{
	bool value;
	const TCHAR* discription;
};

AudioDataTypes g_AudioDataTypes[] = {
	{AVAudioCtrl::AUDIO_DATA_SOURCE_MIC, L"麦克风输出"},
	{AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND, L"发送混音输入"},
	{AVAudioCtrl::AUDIO_DATA_SOURCE_SEND, L"发送输出"},
	{AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY, L"扬声器混音输入"},
	{AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY, L"扬声器输出"},
	{AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM, L"远端音频输出"},
};

AudioDataSampleRate g_AudioDataSampleRates[] = {
	{8000, L"8000"},
	{16000, L"16000"},
	{32000, L"32000"},
	{44100, L"44100"},
	{48000, L"48000"},
};

AudioDataChannelNumState g_AudioDataChannelNumStates[] = {
	{1, L"1"},
	{2, L"2"},
};


void LoginCallBack::OnSuccess() 
{
	::PostMessage(GetMainHWnd(), WM_ON_LOGIN, (WPARAM)AV_OK, 0);
};

void LoginCallBack::OnError(int retCode, const std::string &desc) 
{
	ErrorInfo *pInfo = new ErrorInfo;
	pInfo->retCode = retCode;
	pInfo->desc = desc.c_str();

	::PostMessage(GetMainHWnd(), WM_ON_LOGIN, (WPARAM)AV_ERR_FAILED, (LPARAM)pInfo);
};

void LogoutCallBack::OnSuccess() 
{
	::PostMessage(GetMainHWnd(), WM_ON_LOGOUT, (WPARAM)AV_OK, 0);
};

void LogoutCallBack::OnError(int retCode, const std::string &desc) 
{
	::PostMessage(GetMainHWnd(), WM_ON_LOGOUT, (WPARAM)AV_ERR_FAILED, retCode);
};

void PushVideoStartCallBack::OnSuccess(TIMStreamRsp& value)
{
	TIMStreamRsp *rsp = new TIMStreamRsp(value);
	::PostMessage(GetMainHWnd(), WM_ON_PUSH_VIDEO_START, (WPARAM)AV_OK, (LPARAM)rsp);
}

void PushVideoStartCallBack::OnError(int code, const std::string &desc)
{
	::PostMessage(GetMainHWnd(), WM_ON_PUSH_VIDEO_START, (WPARAM)AV_ERR_FAILED, code);
}

void PushVideoStopCallBack::OnSuccess()
{
	::PostMessage(GetMainHWnd(), WM_ON_PUSH_VIDEO_STOP, (WPARAM)AV_OK, 0);
}

void PushVideoStopCallBack::OnError(int code, const std::string &desc)
{
	::PostMessage(GetMainHWnd(), WM_ON_PUSH_VIDEO_STOP, (WPARAM)AV_ERR_FAILED, code);
}

void RecordVideoStartCallBack::OnSuccess()
{
	::PostMessage(GetMainHWnd(), WM_ON_RECORD_VIDEO_START, (WPARAM)AV_OK, 0);
}

void RecordVideoStartCallBack::OnError(int code, const std::string &desc)
{
	::PostMessage(GetMainHWnd(), WM_ON_RECORD_VIDEO_START, (WPARAM)AV_ERR_FAILED, code);
}

void RecordVideoStopCallBack::OnSuccess(CallBackDataType data)
{
	std::string* file_id = new std::string;
	if (data.size() > 0)
	{
		*file_id = data.front();
	}
	::PostMessage(GetMainHWnd(), WM_ON_RECORD_VIDEO_STOP, (WPARAM)AV_OK, (LPARAM)file_id);
}

void RecordVideoStopCallBack::OnError(int code, const std::string &desc)
{
	::PostMessage(GetMainHWnd(), WM_ON_RECORD_VIDEO_STOP, (WPARAM)AV_ERR_FAILED, code);
}

DialogQAVSDKDemo::DialogQAVSDKDemo(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogQAVSDKDemo::IDD, pParent)
,m_sdkWrapper(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_isEnableMic = true;
	m_isEnablePlayer = true;
	m_isEnableScreenShareSend = true;
	m_isEnableCamera = true;
	m_isDeviceTestEnableMic = true;
	m_isDeviceTestEnablePlayer = true;
	m_isDeviceTestEnableCamera = true;
	m_isExitDemo = false;

	m_operationTipsLogining = _T("正在登录...");
	m_operationTipsLogouting = _T("正在退出登录...");
	m_operationTipsEnterRooming = _T("正在进入房间...");
	m_operationTipsExitRooming = _T("正在退出房间...");
	m_operationTipsOpenCameraing = _T("正在打开摄像头...");
	m_operationTipsCloseCameraing = _T("正在关闭摄像头...");
	m_operationTipsReqeustViewing = _T("正在请求画面...");
	m_operationTipsCancelViewing = _T("正在取消画面...");

	ConfigInfoMgr::GetInst()->LoadConfigInfo();
	m_pYUVData = NULL;
	m_nYUVWidth = 0;
	m_nYUVHeight = 0;
	m_bEnablePreTreat = FALSE;

	m_pYUVCapData = NULL;
	m_nYUVCapWidth = 0;
	m_nYUVCapHeight = 0;
	m_bEnableExCap = FALSE;

	m_pYuvInputFile = NULL;
	m_pYuvOutputFile = NULL;
	m_rectYuvOutputWidth = 0;
	m_rectYuvOutputHeight = 0;

	m_p1v1VideoRenderDlg = NULL;
	m_pAVQualityTipsDlg = NULL;

	m_isVideoPushing = FALSE;
	m_isVideoRecording = FALSE;
	m_uPushVideoChID = 0;
}

DialogQAVSDKDemo::~DialogQAVSDKDemo()
{
	ConfigInfoMgr::GetInst()->SaveConfigInfo();

    if (m_pYUVData)
    {
        delete m_pYUVData;
        m_pYUVData = NULL;
    }

    if (m_pYUVCapData)
    {
        delete m_pYUVCapData;
        m_pYUVCapData = NULL;
    }

	if (m_p1v1VideoRenderDlg)
	{
		delete m_p1v1VideoRenderDlg;
		m_p1v1VideoRenderDlg = NULL;
	}

	if (m_pAVQualityTipsDlg)
	{
		delete m_pAVQualityTipsDlg;
		m_pAVQualityTipsDlg = NULL;
	}	
}

void DialogQAVSDKDemo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OPEN_USER_MANUAL, m_btnOpenUserManual);
	DDX_Control(pDX, IDC_COMBO_SCENE_TYPE_LIST, m_comboSceneTypeList);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_TYPE, m_editAccountType);
	DDX_Control(pDX, IDC_EDIT_APP_ID_AT_3RD, m_editAppIdAt3rd);
	DDX_Control(pDX, IDC_EDIT_SDK_APP_ID, m_editSdkAppId);
	DDX_Control(pDX, IDC_EDIT_ROOM_ID, m_editRoomId);
	DDX_Control(pDX, IDC_EDIT_ROOM_ROLE, m_editRoomRole);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT_LIST, m_comboAccountList);
	DDX_Control(pDX, IDC_COMBO_CHANNEL_LIST, m_comboChannelList);  //自己加的
	DDX_Control(pDX, IDC_LIST_ENDPOINT_LIST, m_listEndpointList);	
	DDX_Control(pDX, IDC_COMBO_YUV_TYPE, m_comboVideoInputType);
	DDX_Control(pDX, IDC_VIEW_LOCAL, m_viewLocalVideoRender);
	DDX_Control(pDX, IDC_VIEW_ROMOTE, m_viewBigVideoRender);

	DDX_Control(pDX, IDC_VIEW_SCREEN_SHARE, m_viewScreenShareRender);
	DDX_Control(pDX, IDC_BTN_ENABLE_TEST_ENV, m_btnEnableTestEnv);
	DDX_Control(pDX, IDC_BTN_SNAPSHOT, m_btnSnapShot);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnLogin);
	DDX_Control(pDX, IDC_BTN_LOGOUT, m_btnLogout);
	DDX_Control(pDX, IDC_BTN_ENTER_ROOM, m_btnEnterRoom);
	DDX_Control(pDX, IDC_BTN_EXIT_ROOM, m_btnExitRoom);
	DDX_Control(pDX, IDC_BTN_ADD_ACCOUNT, m_btnAddAccount);
	DDX_Control(pDX, IDC_BTN_ADD_CHANNEL, m_btnAddChannel);  //自己加的
	DDX_Control(pDX, IDC_BTN_DELETE_ACCOUNT, m_btnDeleteAccount);
	DDX_Control(pDX, IDC_BTN_DELETE_CHANNEL, m_btnDeleteChannel);  //自己加的
	DDX_Control(pDX, IDC_COMBO_CAMERA_LIST, m_comboCameraList);
	DDX_Control(pDX, IDC_COMBO_MIC_LIST, m_comboMicList);
	DDX_Control(pDX, IDC_COMBO_PLAYER_LIST, m_comboPlayerList);
	DDX_Control(pDX, IDC_BTN_OPEN_CAMERA, m_btnOpenCamera);
	DDX_Control(pDX, IDC_BTN_CLOSE_CAMERA, m_btnCloseCamera);
	DDX_Control(pDX, IDC_BTN_OPEN_MIC, m_btnOpenMic);
	DDX_Control(pDX, IDC_BTN_CLOSE_MIC, m_btnCloseMic);
	DDX_Control(pDX, IDC_BTN_OPEN_PLAYER, m_btnOpenPlayer);
	DDX_Control(pDX, IDC_BTN_CLOSE_PLAYER, m_btnClosePlayer);
	DDX_Control(pDX, IDC_BTN_OPEN_SCREEN_SHARE_SEND, m_btnOpenScreenShareSend);
	DDX_Control(pDX, IDC_BTN_CLOSE_SCREEN_SHARE_SEND, m_btnCloseScreenShareSend);
	DDX_Control(pDX, IDC_CHECK_YUV_OUTPUT, m_btnCheckYuvOutput);
	DDX_Control(pDX, IDC_CHK_ENABLE_AEC, m_chkAudioEnableAEC);
	DDX_Control(pDX, IDC_CHK_ENABLE_NS, m_chkAudioEnableNS);
	DDX_Control(pDX, IDC_CHK_ENABLE_BOOST, m_chkAudioEnableBoost);
	DDX_Control(pDX, IDC_SLIDER_MIC_VOLUME, m_sliderAudioMicVolume);
	DDX_Control(pDX, IDC_STATIC_MIC_CUR_VOLUME, m_staticMicCurVolume);
	DDX_Control(pDX, IDC_SLIDER_PLAYER_VOLUME, m_sliderAudioPlayerVolume);
	DDX_Control(pDX, IDC_STATIC_PLAYER_CUR_VOLUME, m_staticPlayerCurVolume);

	DDX_Control(pDX, IDC_EDIT_PLAYER_PATH, m_editPlayerPath);
	DDX_Control(pDX, IDC_EDIT_MEDIA_PATH, m_editMediaPath);
	DDX_Control(pDX, IDC_EDIT_YUV_INPUT_PATH, m_editYuvInputPath);
	DDX_Control(pDX, IDC_EDIT_YUV_OUTPUT_WIDTH, m_editYuvInputWidth);
	DDX_Control(pDX, IDC_EDIT_YUV_OUTPUT_HEIGHT, m_editYuvInputHeight);
	DDX_Control(pDX, IDC_SLIDER_ACCOMPANY_VOLUME, m_sliderAccompanyVolume);
	DDX_Control(pDX, IDC_STATIC_ACCOMPANY_CUR_VOLUME, m_staticAcompanyCurVolume);
	DDX_Control(pDX, IDC_BTN_OPEN_AV_QUALITY_TIPS, m_btnOpenAVQualityTips);
	DDX_Control(pDX, IDC_BTN_CLOSE_AV_QUALITY_TIPS, m_btnCloseAVQualityTips);
	//DDX_Control(pDX, IDC_STATIC_SDK_VERSION, m_staticSDKVersion);
	DDX_Control(pDX, IDC_STATIC_OPERATION_TIPS, m_staticOperationTips);
	DDX_Control(pDX, IDC_BTN_PROAUT, m_btnProtoAuth);
	DDX_Control(pDX, IDC_EDIT_CHANGE_AUTH_BUFFER, m_editProtoAuthBuffer);

	DDX_Control(pDX, IDC_BTN_START_DEVICE_TEST, m_btnStartDeviceTest);
	DDX_Control(pDX, IDC_BTN_STOP_DEVICE_TEST, m_btnStopDeviceTest);
	DDX_Control(pDX, IDC_COMBO_DEVICE_TEST_CAMERA_LIST, m_comboDeviceTestCameraList);
	DDX_Control(pDX, IDC_COMBO_DEVICE_TEST_MIC_LIST, m_comboDeviceTestMicList);
	DDX_Control(pDX, IDC_COMBO_DEVICE_TEST_PLAYER_LIST, m_comboDeviceTestPlayerList);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_OPEN_CAMERA, m_btnDeviceTestOpenCamera);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_CLOSE_CAMERA, m_btnDeviceTestCloseCamera);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_OPEN_MIC, m_btnDeviceTestOpenMic);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_CLOSE_MIC, m_btnDeviceTestCloseMic);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_OPEN_PLAYER, m_btnDeviceTestOpenPlayer);
	DDX_Control(pDX, IDC_BTN_DEVICE_TEST_CLOSE_PLAYER, m_btnDeviceTestClosePlayer);
	DDX_Control(pDX, IDC_SLIDER_DEVICE_TEST_MIC_VOLUME, m_sliderDeviceTestAudioMicVolume);
	DDX_Control(pDX, IDC_SLIDER_DEVICE_TEST_PLAYER_VOLUME, m_sliderDeviceTestAudioPlayerVolume);

	DDX_Control(pDX, IDC_CHECK_VIDEO_SEND, m_btnCheckHasVideoSendAuth);
	DDX_Control(pDX, IDC_CHECK_AUDIO_SEND, m_btnCheckHasAudioSendAuth);
	DDX_Control(pDX, IDC_CHECK_VIDEO_REV, m_btnCheckHasVideoRevAuth);
	DDX_Control(pDX, IDC_CHECK_AUDIO_REV, m_btnCheckHasAudioRevAuth);
	DDX_Control(pDX, IDC_CHECK_IS_AUTO_CREATE_SDK_ROOM, m_chkIsAutoCreateSDKRoom);

	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_MIC, m_btnCheckAudioDataMic);
	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_MIX_TO_SEND, m_btnCheckAudioDataMixToSend);
	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_SEND, m_btnCheckAudioDataSend);
	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_NETSREAM, m_btnCheckAudioDataNetStream);
	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_MIX_TO_PLAY, m_btnCheckAudioDataMixToPlay);
	DDX_Control(pDX, IDC_CHECK_AUDIO_DATA_PLAY, m_btnCheckAudioDataPlay);
	DDX_Control(pDX, IDC_EDIT_AUDIO_DATA_MIX_TO_SEND, m_editMixToSendPath);
	DDX_Control(pDX, IDC_EDIT_AUDIO_DATA_MIX_TO_PLAY, m_editMixToPlayPath);

	DDX_Control(pDX, IDC_SLIDER_AUDIO_DATA_VOLUME, m_sliderAudioDataVolume);
	DDX_Control(pDX, IDC_COMBO_AUDIO_DATA_TYPE, m_comboAudioDataType);
	DDX_Control(pDX, IDC_COMBO_AUDIO_DATA_SR, m_comboAudioDataSampleRate);
	DDX_Control(pDX, IDC_COMBO_AUDIO_DATA_CN, m_comboAudioDataChannelNum);

	DDX_Control(pDX, IDC_EDIT_SCREEN_SHARE_CAPTURE_LEFT, m_editScreenShareCaptureLeft);
	DDX_Control(pDX, IDC_EDIT_SCREEN_SHARE_CAPTURE_TOP, m_editScreenShareCaptureTop);
	DDX_Control(pDX, IDC_EDIT_SCREEN_SHARE_CAPTURE_RIGHT, m_editScreenShareCaptureRight);
	DDX_Control(pDX, IDC_EDIT_SCREEN_SHARE_CAPTURE_BOTTOM, m_editScreenShareCaptureBottom);
	DDX_Control(pDX, IDC_EDIT_SCREEN_SHARE_CAPTURE_FPS, m_editScreenShareCaptureFps);
	DDX_Control(pDX, IDC_COMBO_SCREEN_SHARE_MODE_LIST, m_comboScreenShareModeList);

	DDX_Control(pDX, IDC_BTN_PUSH_VIDEO_REQ, m_btnPushVideoReq);
	DDX_Control(pDX, IDC_COMBO_PUSH_VIDEO_DATA_TYPE, m_comboPushVideoDataType);
	DDX_Control(pDX, IDC_COMBO_PUSH_VIDEO_ENCODE_TYPE, m_comboPushVideoEncodeType);
	DDX_Control(pDX, IDC_BTN_RECORD_VIDEO_REQ, m_btnRecordVideoReq);
	DDX_Control(pDX, IDC_COMBO_RECORD_VIDEO_DATA_TYPE, m_comboRecordVideoDataType);

	DDX_Control(pDX, IDC_BTN_OPEN_EXTERNAL_CAPTURE, m_btnOpenExternalCapture);
	DDX_Control(pDX, IDC_BTN_CLOSE_EXTERNAL_CAPTURE, m_btnCloseExternalCapture);
	DDX_Control(pDX, IDC_EDIT_CHANGE_AUTH_BIT, m_editProtoAuthBit);
	DDX_Control(pDX, IDC_EDIT_FILEPATH, para_file_path);
}

BEGIN_MESSAGE_MAP(DialogQAVSDKDemo, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()

	//ON_MESSAGE_VOID(WM_CLOSE, OnCloseDemo)
	ON_MESSAGE(WM_ON_LOGIN, OnLogin)
	ON_MESSAGE(WM_ON_LOGOUT, OnLogout)
	ON_MESSAGE(WM_ON_START_CONTEXT, OnStartContext)
	ON_MESSAGE(WM_ON_STOP_CONTEXT, OnStopContext)
	ON_MESSAGE(WM_ON_ENTER_ROOM, OnEnterRoom)
	ON_MESSAGE(WM_ON_EXIT_ROOM, OnExitRoom)
	ON_MESSAGE(WM_ON_ENDPOINTS_UPDATE_INFO, OnEnpointsUpdateInfo)
	ON_MESSAGE(WM_ON_REQUEST_VIEW_LIST, OnRequestViewList)
	ON_MESSAGE(WM_ON_CANCEL_ALL_VIEW, OnCancelAllView)
	ON_MESSAGE(WM_ON_MIC_OPERATION, OnMicOperation)
	ON_MESSAGE(WM_ON_PLAYER_OPERATION, OnPlayerOperation)
	ON_MESSAGE(WM_ON_SCREEN_SHARE_SEND_OPERATION, OnScreenShareSendOperation)
	ON_MESSAGE(WM_ON_ACCOMPANY_OPERATION, OnAccompanyOperation)
	ON_MESSAGE(WM_ON_CAMERA_OPERATION, OnCameraOperation)
	ON_MESSAGE(WM_ON_AUDIO_DEVICE_DETECT, OnAudioDeviceDetect)
	ON_MESSAGE(WM_LBUTTONDBLCLK, OnLButtonDBClick)
	ON_MESSAGE(WM_ENDPOINT_MENU_ITEM, OnEndpointMenuItem)
	ON_MESSAGE(DM_GETDEFID, OnGetDefId)
	ON_MESSAGE(WM_ON_START_DEVICE_TEST, OnStartDeviceTest)
	ON_MESSAGE(WM_ON_STOP_DEVICE_TEST, OnStopDeviceTest)
	ON_MESSAGE(WM_ON_DEVICE_TEST_MIC_OPERATION, OnDeviceTestMicOperation)
	ON_MESSAGE(WM_ON_DEVICE_TEST_PLAYER_OPERATION, OnDeviceTestPlayerOperation)
	ON_MESSAGE(WM_ON_DEVICE_TEST_CAMERA_OPERATION, OnDeviceTestCameraOperation)

	ON_MESSAGE(WM_ON_PUSH_VIDEO_START, OnPushVideoStart)  //绑定一个自定义的消息和该消息的响应函数
	ON_MESSAGE(WM_ON_PUSH_VIDEO_STOP, OnPushVideoStop)
	ON_MESSAGE(WM_ON_RECORD_VIDEO_START, OnRecordVideoStart)
	ON_MESSAGE(WM_ON_RECORD_VIDEO_STOP, OnRecordVideoStop)

	ON_BN_CLICKED(IDC_BTN_LOGIN, &DialogQAVSDKDemo::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_LOGOUT, &DialogQAVSDKDemo::OnBnClickedBtnLogout)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AEC, &DialogQAVSDKDemo::OnBnClickedChkEnableAec)
	ON_BN_CLICKED(IDC_CHK_ENABLE_NS, &DialogQAVSDKDemo::OnBnClickedChkEnableNs)
	ON_BN_CLICKED(IDC_CHK_ENABLE_BOOST, &DialogQAVSDKDemo::OnBnClickedChkEnableBoost)
	ON_BN_CLICKED(IDC_BTN_ENTER_ROOM, &DialogQAVSDKDemo::OnBnClickedBtnEnterRoom)
	ON_BN_CLICKED(IDC_BTN_EXIT_ROOM, &DialogQAVSDKDemo::OnBnClickedBtnExitRoom)
	ON_BN_CLICKED(IDC_BTN_ADD_ACCOUNT, &DialogQAVSDKDemo::OnBnClickedBtnAddAccount)
	ON_BN_CLICKED(IDC_BTN_ADD_CHANNEL, &DialogQAVSDKDemo::OnBnClickedBtnAddChannel)  //自己加的
	ON_BN_CLICKED(IDC_BTN_DELETE_ACCOUNT, &DialogQAVSDKDemo::OnBnClickedBtnDeleteAccount)
	ON_BN_CLICKED(IDC_BTN_DELETE_CHANNEL, &DialogQAVSDKDemo::OnBnClickedBtnDeleteChannel)  //自己加的
	ON_BN_CLICKED(IDC_BTN_ADD_APP, &DialogQAVSDKDemo::OnBnClickedBtnAddApp)
	ON_BN_CLICKED(IDC_BTN_DELETE_APP, &DialogQAVSDKDemo::OnBnClickedBtnDeleteApp)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboCameraList)
	ON_CBN_SELCHANGE(IDC_COMBO_MIC_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboMicList)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYER_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboPlayerList)
	ON_BN_CLICKED(IDC_BTN_OPEN_CAMERA, &DialogQAVSDKDemo::OnBnClickedBtnOpenCamera)
	ON_BN_CLICKED(IDC_BTN_CLOSE_CAMERA, &DialogQAVSDKDemo::OnBnClickedBtnCloseCamera)
	ON_BN_CLICKED(IDC_BTN_OPEN_MIC, &DialogQAVSDKDemo::OnBnClickedBtnOpenMic)
	ON_BN_CLICKED(IDC_BTN_CLOSE_MIC, &DialogQAVSDKDemo::OnBnClickedBtnCloseMic)
	ON_BN_CLICKED(IDC_BTN_OPEN_PLAYER, &DialogQAVSDKDemo::OnBnClickedBtnOpenPlayer)
	ON_BN_CLICKED(IDC_BTN_CLOSE_PLAYER, &DialogQAVSDKDemo::OnBnClickedBtnClosePlayer)
	ON_BN_CLICKED(IDC_BTN_START_ACCOMPANY, &DialogQAVSDKDemo::OnBnClickedBtnStartAccompany)
	ON_BN_CLICKED(IDC_BTN_STOP_ACCOMPANY, &DialogQAVSDKDemo::OnBnClickedBtnStopAccompany)
  
	ON_BN_CLICKED(IDC_BTN_OPEN_AV_QUALITY_TIPS, &DialogQAVSDKDemo::OnBnClickedBtnOpenAVQualityTips)
	ON_BN_CLICKED(IDC_BTN_CLOSE_AV_QUALITY_TIPS, &DialogQAVSDKDemo::OnBnClickedBtnCloseAVQualityTips)
 
	ON_EN_CHANGE(IDC_EDIT_ROOM_ID, &DialogQAVSDKDemo::OnEnChangeEditRoomId)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENE_TYPE_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboSceneTypeList)
	ON_BN_CLICKED(IDC_BTN_ENABLE_TEST_ENV, &DialogQAVSDKDemo::OnBnClickedBtnEnableTestEnv)
	ON_BN_CLICKED(IDC_BTN_OPEN_USER_MANUAL, &DialogQAVSDKDemo::OnBnClickedBtnOpenUserManual)
	ON_BN_CLICKED(IDC_BTN_SNAPSHOT, &DialogQAVSDKDemo::OnBnClickedBtnSnapShot)
	ON_BN_CLICKED(IDC_BTN_PROAUT, &DialogQAVSDKDemo::OnBnClickedBtnProaut)

	ON_BN_CLICKED(IDC_EnablePreTreat, &DialogQAVSDKDemo::OnBnClickedEnablepretreat)
	ON_BN_CLICKED(IDC_DisablePreTreat, &DialogQAVSDKDemo::OnBnClickedDisablepretreat)

	ON_BN_CLICKED(IDC_CHECK_YUV_OUTPUT, &DialogQAVSDKDemo::OnBnClickedBtnCheckYuvOutput)

	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_MIC, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataMic)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_MIX_TO_SEND, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataMixToSend)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_SEND, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataSend)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_NETSREAM, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataNetStream)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_MIX_TO_PLAY, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataMixToPlay)
	ON_BN_CLICKED(IDC_CHECK_AUDIO_DATA_PLAY, &DialogQAVSDKDemo::OnBnClickedBtnAudioDataPlay)

	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_DATA_TYPE, &DialogQAVSDKDemo::OnCbnSelchangeComboAudioDataType)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_DATA_SR, &DialogQAVSDKDemo::OnCbnSelchangeComboAudioDataParam)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_DATA_CN, &DialogQAVSDKDemo::OnCbnSelchangeComboAudioDataParam)

	ON_CBN_SELCHANGE(IDC_SLIDER_AUDIO_DATA_VOLUME, &DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestCameraList)
	
	ON_BN_CLICKED(IDC_BTN_START_DEVICE_TEST, &DialogQAVSDKDemo::OnBnClickedBtnStartDeviceTest)
	ON_BN_CLICKED(IDC_BTN_STOP_DEVICE_TEST, &DialogQAVSDKDemo::OnBnClickedBtnStopDeviceTest)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TEST_CAMERA_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestCameraList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TEST_MIC_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestMicList)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE_TEST_PLAYER_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestPlayerList)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_OPEN_CAMERA, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenCamera)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_CLOSE_CAMERA, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestCloseCamera)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_OPEN_MIC, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenMic)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_CLOSE_MIC, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestCloseMic)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_OPEN_PLAYER, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenPlayer)
	ON_BN_CLICKED(IDC_BTN_DEVICE_TEST_CLOSE_PLAYER, &DialogQAVSDKDemo::OnBnClickedBtnDeviceTestClosePlayer)

	ON_BN_CLICKED(IDC_BTN_OPEN_SCREEN_SHARE_SEND, &DialogQAVSDKDemo::OnBnClickedBtnOpenScreenShareSend)
	ON_BN_CLICKED(IDC_BTN_CLOSE_SCREEN_SHARE_SEND, &DialogQAVSDKDemo::OnBnClickedBtnCloseScreenShareSend)
	ON_CBN_SELCHANGE(IDC_COMBO_SCREEN_SHARE_MODE_LIST, &DialogQAVSDKDemo::OnCbnSelchangeComboScreenShareModeList)
	ON_BN_CLICKED(IDC_BTN_OPEN_1V1_VIEW, &DialogQAVSDKDemo::OnBnClickedBtnOpen1v1View)
	ON_BN_CLICKED(IDC_BTN_CLOSE_1V1_VIEW, &DialogQAVSDKDemo::OnBnClickedBtnClose1v1View)

	ON_BN_CLICKED(IDC_BTN_PUSH_VIDEO_REQ, &DialogQAVSDKDemo::OnClickedBtnPushVideoReq)
	ON_BN_CLICKED(IDC_BTN_RECORD_VIDEO_REQ, &DialogQAVSDKDemo::OnClickedBtnRecordVideoReq)
	ON_BN_CLICKED(IDC_BTN_OPEN_EXTERNAL_CAPTURE, &DialogQAVSDKDemo::OnBnClickedBtnOpenExteralCapture)
	ON_BN_CLICKED(IDC_BTN_CLOSE_EXTERNAL_CAPTURE, &DialogQAVSDKDemo::OnBnClickedBtnCloseExteralCapture)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_PARA, &DialogQAVSDKDemo::OnBnClickedButtonSearchPara)
	ON_STN_CLICKED(IDC_VIEW_SCREEN_SHARE, &DialogQAVSDKDemo::OnStnClickedViewScreenShare)
	ON_BN_CLICKED(IDC_CHECK_IS_AUTO_CREATE_SDK_ROOM, &DialogQAVSDKDemo::OnBnClickedCheckIsAutoCreateSdkRoom)
	ON_STN_CLICKED(IDC_STATIC_ROOM_ID, &DialogQAVSDKDemo::OnStnClickedStaticRoomId)
	ON_STN_CLICKED(IDC_STATIC_CAMERA, &DialogQAVSDKDemo::OnStnClickedStaticCamera)
	END_MESSAGE_MAP()

BOOL DialogQAVSDKDemo::OnInitDialog()  //消息响应函数，响应WM_INITDIALOG消息，发送此消息以允许初始化对话框中的控件，或者是创建新控件
{
	CDialogEx::OnInitDialog();

	AppInfolist appInfoList = ConfigInfoMgr::GetInst()->GetAppInfoList();	
	if (appInfoList.size() > 0)
	{		
	    m_curSceneType = appInfoList[0].sceneType;
	    m_comboSceneTypeList.SetWindowTextW(appInfoList[0].sceneType);
	    m_comboSceneTypeList.SetCurSel(0);
	    m_editAccountType.SetWindowTextW(appInfoList[0].accountType);
	    m_editAppIdAt3rd.SetWindowTextW(appInfoList[0].appIdAt3rd);
	    m_editSdkAppId.SetWindowTextW(appInfoList[0].sdkAppId);		
	}
	else
	{
		if(DEFAULT_SCENE == SCENE_IM_EXPERIENCE)
		{
			m_curSceneType = _T("实时通信场景体验版");
			m_comboSceneTypeList.SetWindowTextW(m_curSceneType);
			m_editAccountType.SetWindowTextW(_T("107"));
			m_editAppIdAt3rd.SetWindowTextW(_T("1104620500"));
			m_editSdkAppId.SetWindowTextW(_T("1104620500"));
		}
		else if(DEFAULT_SCENE == SCENE_ANCHOR_EXPERIENCE)
		{
			m_curSceneType = _T("主播场景体验版");
			m_comboSceneTypeList.SetWindowTextW(m_curSceneType);
			m_editAccountType.SetWindowTextW(_T("107"));
			m_editAppIdAt3rd.SetWindowTextW(_T("0"));
			m_editSdkAppId.SetWindowTextW(_T("1104062745"));
		}
	}

	m_comboVideoInputType.ResetContent();
	m_comboVideoInputType.SetWindowTextW(_T(""));
	m_comboVideoInputType.InsertString(0, _T("I420"));
	m_comboVideoInputType.InsertString(1, _T("RGB24"));
	m_comboVideoInputType.SetCurSel(0);

	m_comboAudioDataType.ResetContent();
	m_comboAudioDataSampleRate.ResetContent();
	m_comboAudioDataChannelNum.ResetContent();

	m_comboPushVideoDataType.InsertString(0, _T("摄像头"));
	m_comboPushVideoDataType.InsertString(1, _T("辅流"));
	m_comboPushVideoEncodeType.InsertString(0, _T("HLS"));
	m_comboPushVideoEncodeType.InsertString(1, _T("RTMP"));
	m_comboRecordVideoDataType.InsertString(0, _T("摄像头"));
	m_comboRecordVideoDataType.InsertString(1, _T("辅流"));
	for (int i = 0; i < sizeof(g_AudioDataTypes) / sizeof(g_AudioDataTypes[0]); i++)
	{
		m_comboAudioDataType.InsertString(i, g_AudioDataTypes[i].discription);
	}
	for (int i = 0; i < sizeof(g_AudioDataSampleRates) / sizeof(g_AudioDataSampleRates[0]); i++)
	{
		m_comboAudioDataSampleRate.InsertString(i, g_AudioDataSampleRates[i].discription);
	}
	for (int i = 0; i < sizeof(g_AudioDataChannelNumStates) / sizeof(g_AudioDataChannelNumStates[0]); i++)
	{
		m_comboAudioDataChannelNum.InsertString(i, g_AudioDataChannelNumStates[i].discription);
	}

	m_comboAudioDataType.SetCurSel(0);
	m_comboAudioDataSampleRate.SetCurSel(0);
	m_comboAudioDataChannelNum.SetCurSel(0);

	m_sliderAudioDataVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAudioDataVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);

	m_sliderAudioMicVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAudioMicVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAudioPlayerVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAudioPlayerVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAccompanyVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderAccompanyVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);

	m_sliderDeviceTestAudioMicVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderDeviceTestAudioMicVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);
	m_sliderDeviceTestAudioPlayerVolume.SetRange(MIN_AUDIO_DEVICE_VOLUME, MAX_AUDIO_DEVICE_VOLUME);
	m_sliderDeviceTestAudioPlayerVolume.SetTicFreq(MAX_AUDIO_DEVICE_VOLUME);

	
	CRect mainDialogRect; 
	GetWindowRect(&mainDialogRect);

	mainDialogRect.BottomRight() = CPoint(::GetSystemMetrics(SM_CXSCREEN),::GetSystemMetrics(SM_CYSCREEN));
    mainDialogRect.TopLeft() = CPoint(0,0);
    MoveWindow(&mainDialogRect);

	m_isTestEnv = false;//默认设置为测试环境/正式环境
	m_isLogin = false;
	m_isEnterRoom = false;
	m_isStartDeviceTest = false;

	m_btnSnapShot.EnableWindow(TRUE);
	m_btnEnterRoom.EnableWindow(TRUE);
	m_btnExitRoom.EnableWindow(TRUE);
	m_btnOpenCamera.EnableWindow(TRUE);
	m_btnCloseCamera.EnableWindow(TRUE);
	m_btnOpenMic.EnableWindow(TRUE);
	m_btnCloseMic.EnableWindow(TRUE);
	m_btnOpenPlayer.EnableWindow(TRUE);
	m_btnClosePlayer.EnableWindow(TRUE);
	m_btnOpenScreenShareSend.EnableWindow(TRUE);
	m_btnCloseScreenShareSend.EnableWindow(TRUE);
	m_btnOpenExternalCapture.EnableWindow(TRUE);
	m_btnCloseExternalCapture.EnableWindow(TRUE);

	m_btnCheckHasVideoSendAuth.SetCheck(1);
	m_btnCheckHasAudioSendAuth.SetCheck(1);
	m_btnCheckHasVideoRevAuth.SetCheck(1);
	m_btnCheckHasAudioRevAuth.SetCheck(1);
	m_chkIsAutoCreateSDKRoom.SetCheck(1);

	m_btnStartDeviceTest.EnableWindow(TRUE);
	m_btnStopDeviceTest.EnableWindow(TRUE);
	m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
	m_btnDeviceTestCloseCamera.EnableWindow(TRUE);
	m_btnDeviceTestOpenMic.EnableWindow(TRUE);
	m_btnDeviceTestCloseMic.EnableWindow(TRUE);
	m_btnDeviceTestOpenPlayer.EnableWindow(TRUE);
	m_btnDeviceTestClosePlayer.EnableWindow(TRUE);

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnEnterRoom.EnableWindow(FALSE);
	m_btnExitRoom.EnableWindow(FALSE);
	m_btnOpenCamera.EnableWindow(FALSE);
	m_btnCloseCamera.EnableWindow(FALSE);
	m_btnOpenMic.EnableWindow(FALSE);
	m_btnCloseMic.EnableWindow(FALSE);
	m_btnOpenPlayer.EnableWindow(FALSE);
	m_btnClosePlayer.EnableWindow(FALSE);
	m_btnOpenScreenShareSend.EnableWindow(FALSE);
	m_btnCloseScreenShareSend.EnableWindow(FALSE);

	m_btnStartDeviceTest.EnableWindow(FALSE);
	m_btnStopDeviceTest.EnableWindow(FALSE);
	m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
	m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
	m_btnDeviceTestOpenMic.EnableWindow(FALSE);
	m_btnDeviceTestCloseMic.EnableWindow(FALSE);
	m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
	m_btnDeviceTestClosePlayer.EnableWindow(FALSE);

	m_btnCheckAudioDataMic.EnableWindow(FALSE);
	m_btnCheckAudioDataMixToSend.EnableWindow(FALSE);
	m_btnCheckAudioDataSend.EnableWindow(FALSE);
	m_btnCheckAudioDataNetStream.EnableWindow(FALSE);
	m_btnCheckAudioDataMixToPlay.EnableWindow(FALSE);
	m_btnCheckAudioDataPlay.EnableWindow(FALSE);
	m_sliderAudioDataVolume.EnableWindow(FALSE);
	m_comboAudioDataType.EnableWindow(FALSE);
	m_comboAudioDataSampleRate.EnableWindow(FALSE);
	m_comboAudioDataChannelNum.EnableWindow(FALSE);

	m_btnOpenExternalCapture.EnableWindow(FALSE);
	m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif

	if(m_isTestEnv)
	{
		m_btnEnableTestEnv.SetWindowTextW(_T("进入正式环境"));		
	}
	else
	{
		m_btnEnableTestEnv.SetWindowTextW(_T("进入测试环境"));
	}

	m_btnProtoAuth.EnableWindow(FALSE);
	m_editProtoAuthBuffer.EnableWindow(FALSE);

	m_viewBigVideoRender.Init(VIDEO_RENDER_BIG_VIEW_WIDTH, VIDEO_RENDER_BIG_VIEW_HEIGHT, COLOR_FORMAT_RGB24);
	m_viewLocalVideoRender.Init(VIDEO_RENDER_SMALL_VIEW_WIDTH, VIDEO_RENDER_SMALL_VIEW_HEIGHT, COLOR_FORMAT_RGB24);	
	m_viewScreenShareRender.Init(VIDEO_RENDER_SMALL_VIEW_WIDTH, VIDEO_RENDER_SMALL_VIEW_HEIGHT, COLOR_FORMAT_RGB24);

	m_viewBigVideoRender.m_videoSrcType = VIDEO_SRC_TYPE_NONE;
	m_viewLocalVideoRender.m_videoSrcType = VIDEO_SRC_TYPE_CAMERA;
	m_viewScreenShareRender.m_videoSrcType = VIDEO_SRC_TYPE_SCREEN;	

	UpdateConfigInfo();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	PrepareYUVData();
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void DialogQAVSDKDemo::OnSysCommand(UINT nID, LPARAM lParam)
{	
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void DialogQAVSDKDemo::OnPaint()
{	
	if (IsIconic())
	{
		
	}
	else
	{
    CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
HCURSOR DialogQAVSDKDemo::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void DialogQAVSDKDemo::TrackMenu(CWnd*pWnd)
{
	CRect rc;
	CMenu menu;

	pWnd->GetWindowRect(&rc);
	menu.LoadMenuW(IDR_MENU_ENDPOINT);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, rc.left, rc.top, this);
}

BOOL DialogQAVSDKDemo::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

LONG DialogQAVSDKDemo::OnGetDefId(WPARAM wp, LPARAM lp)    
{  
	return MAKELONG(0,DC_HASDEFID);    
} 

void DialogQAVSDKDemo::OnClose()  //消息响应函数，响应窗口的WM_CLOSE消息，当关闭按钮被单击的时候发送此消息
{
	m_isExitDemo = true;
	ConfigInfoMgr::GetInst()->SaveConfigInfo();

	if(m_isEnterRoom)
	{
		int retCode = m_sdkWrapper.ExitRoom();
		if(retCode != AV_OK)ClearOperationTips();
		ShowRetCode("ExitRoom", retCode);
	}
	else if(m_isStartDeviceTest)
	{
		int retCode = m_sdkWrapper.StopDeviceTest();
		if(retCode != AV_OK)ClearOperationTips();
		ShowRetCode("StopDeviceTest", retCode);
	}
	else if(m_isLogin)
	{
		int retCode = m_sdkWrapper.StopContext();
		if(retCode != AV_OK)ClearOperationTips();
		ShowRetCode("StopContext", retCode);
	}
	else
	{
		DestroyWindow();
	}
}

void DialogQAVSDKDemo::OnDestroy()  //消息响应函数，响应窗口的WM_DESTROY消息，当一个窗口将被销毁时，发送此消息
{
	PostQuitMessage(WM_QUIT);
}

void DialogQAVSDKDemo::OnTimer(UINT_PTR eventId)
{
	if (eventId == TIMER_ENABLE_QUALITY_TIPS)
	{
		if (m_pAVQualityTipsDlg)
		{
			std::string qualityTips = m_sdkWrapper.GetQualityTips();
			m_pAVQualityTipsDlg->UpdateTips(CString(qualityTips.c_str()));
		}		
	}
	else if (eventId == TIMER_SHOW_OPERRATION_TIPS)
	{
		ClearOperationTips();
	}
  else if (eventId == TIMER_EXTERNAL_CAPTURE)
  {
      CString strYuvInputPath;
      CString strYuvInputWidth;
      CString strYuvInputHeight;
      CString strYuvInputType;

      m_editYuvInputPath.GetWindowText(strYuvInputPath);
      m_editYuvInputWidth.GetWindowText(strYuvInputWidth);
      m_editYuvInputHeight.GetWindowText(strYuvInputHeight);
      m_comboVideoInputType.GetWindowText(strYuvInputType);
	  	  
      if(!m_pYuvInputFile)
      {
        m_pYuvInputFile = fopen(StrWToStrA(strYuvInputPath).c_str(), "rb");
      }

      if(!m_pYuvInputFile) return;

      int nYuvInputWidth = atoi(StrWToStrA(strYuvInputWidth).c_str());  //atoi字符串转换成整型数
      int nYuvInputHeight = atoi(StrWToStrA(strYuvInputHeight).c_str());

      uint32 size = nYuvInputWidth * nYuvInputHeight * 3 / 2;
      std::vector<uint8> data;
      data.resize(size);

      if( size != fread((void*)&data[0] , 1, size, m_pYuvInputFile) )
      {
        fseek(m_pYuvInputFile, 0, SEEK_SET);
        return;
      }
	  //看一下这里
      VideoFrame stFrame;
      stFrame.data = &data[0];
      stFrame.data_size = size;
      stFrame.desc.color_format = StrWToStrA(strYuvInputType) == "I420" ? COLOR_FORMAT_I420 : COLOR_FORMAT_RGB24;
      stFrame.desc.height = nYuvInputHeight;
      stFrame.desc.width = nYuvInputWidth;
      stFrame.desc.rotate = 0;
      int ret = m_sdkWrapper.FillExternalCaptureFrame(&stFrame);
  }

	CWnd::OnTimer(eventId);
}


void DialogQAVSDKDemo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(m_isEnterRoom)
	{
		if((CWnd*)pScrollBar == (CWnd*)&m_sliderAudioMicVolume)  
		{  
			int volume = m_sliderAudioMicVolume.GetPos();  

			if (volume < MIN_AUDIO_DEVICE_VOLUME)
			{
				volume = MIN_AUDIO_DEVICE_VOLUME;
			}
			else if (volume > MAX_AUDIO_DEVICE_VOLUME)
			{
				volume = MAX_AUDIO_DEVICE_VOLUME;
			}
			CString volumeStr;
			volumeStr.Format(_T("%d"), volume);
			m_staticMicCurVolume.SetWindowTextW(volumeStr);
			m_sdkWrapper.SetMicVolume(volume);
		}  
		else if((CWnd*)pScrollBar == (CWnd*)&m_sliderAudioPlayerVolume)  
		{  
			int volume = m_sliderAudioPlayerVolume.GetPos();  

			if (volume < MIN_AUDIO_DEVICE_VOLUME)
			{
				volume = MIN_AUDIO_DEVICE_VOLUME;
			}
			else if (volume > MAX_AUDIO_DEVICE_VOLUME)
			{
			 	volume = MAX_AUDIO_DEVICE_VOLUME;
			}
			CString volumeStr;
			volumeStr.Format(_T("%d"), volume);
			m_staticPlayerCurVolume.SetWindowTextW(volumeStr);
			m_sdkWrapper.SetPlayerVolume(volume);
		}  
		else if((CWnd*)pScrollBar == (CWnd*)&m_sliderAccompanyVolume)  
		{  
			int volume = m_sliderAccompanyVolume.GetPos();  

			if (volume < MIN_AUDIO_DEVICE_VOLUME)
			{
				volume = MIN_AUDIO_DEVICE_VOLUME;
			}
			else if (volume > MAX_AUDIO_DEVICE_VOLUME)
			{
				volume = MAX_AUDIO_DEVICE_VOLUME;
			}
			CString volumeStr;
			volumeStr.Format(_T("%d"), volume);
			m_staticAcompanyCurVolume.SetWindowTextW(volumeStr);
			m_sdkWrapper.SetAccompanyVolume(volume);
		}  
		else if((CWnd*)pScrollBar == (CWnd*)&m_sliderAudioDataVolume)  
		{  
			OnChangeAudioDataVolume();
		}
		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	else if(m_isStartDeviceTest)
	{
		if((CWnd*)pScrollBar == (CWnd*)&m_sliderDeviceTestAudioMicVolume)  
		{  
			int volume = m_sliderDeviceTestAudioMicVolume.GetPos();  

			if (volume < MIN_AUDIO_DEVICE_VOLUME)
			{
				volume = MIN_AUDIO_DEVICE_VOLUME;
			}
			else if (volume > MAX_AUDIO_DEVICE_VOLUME)
			{
				volume = MAX_AUDIO_DEVICE_VOLUME;
			}
			CString volumeStr;
			volumeStr.Format(_T("%d"), volume);
			m_sdkWrapper.DeviceTestSetMicVolume(volume);
		}  
		else if((CWnd*)pScrollBar == (CWnd*)&m_sliderDeviceTestAudioPlayerVolume)  
		{  
			int volume = m_sliderDeviceTestAudioPlayerVolume.GetPos();  

			if (volume < MIN_AUDIO_DEVICE_VOLUME)
			{
				volume = MIN_AUDIO_DEVICE_VOLUME;
			}
			else if (volume > MAX_AUDIO_DEVICE_VOLUME)
			{
				volume = MAX_AUDIO_DEVICE_VOLUME;
			}
			CString volumeStr;
			volumeStr.Format(_T("%d"), volume);
			m_sdkWrapper.DeviceTestSetPlayerVolume(volume);
		} 
		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	
}

LONG DialogQAVSDKDemo::OnLogin(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		//登录成功后，去初始化音视频SDK
		m_sdkWrapper.CreateContext(m_config);
		int retCode = m_sdkWrapper.StartContext();
		ShowRetCode("StartContext", retCode);
	}
	else
	{
		ErrorInfo *pinfo = (ErrorInfo *)lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("登录时出错！错误码：%d, 错误信息：%s."), pinfo->retCode, pinfo->desc);

		delete pinfo;

		ShowMessageBox(errCodeStr);
	}

	return 0;
}

LONG DialogQAVSDKDemo::OnLogout(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		//TODO
	}
	else
	{
		int errCode = lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("退出登录时出错！错误码：%d."), errCode);
		ShowMessageBox(errCodeStr);
	}

	TIMManager::get().Uninit();//退出登录后，释放通信SDK
	if(m_isExitDemo)DestroyWindow();
	return 0;
}

LONG DialogQAVSDKDemo::OnStartContext(WPARAM wParam, LPARAM lParam)
{
	Error nError = (Error)wParam;
	if (AV_OK == nError)
	{
		//音视频SDK初始化成功
		m_isLogin = true;

		//m_btnEnableTestEnv.EnableWindow(FALSE);
		m_comboSceneTypeList.EnableWindow(FALSE);
		m_comboAccountList.EnableWindow(FALSE);
		m_comboChannelList.EnableWindow(FALSE);  //自己加的
		m_btnLogin.EnableWindow(FALSE);
		m_btnLogout.EnableWindow(TRUE);
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(TRUE);
		m_btnExitRoom.EnableWindow(TRUE);
		m_btnStartDeviceTest.EnableWindow(TRUE);
		m_btnStopDeviceTest.EnableWindow(TRUE);

#endif


	}
	else
	{
		m_btnEnableTestEnv.EnableWindow(TRUE);
		m_comboSceneTypeList.EnableWindow(TRUE);
		m_comboAccountList.EnableWindow(TRUE);
		m_comboChannelList.EnableWindow(TRUE);  //自己加的
		m_btnLogin.EnableWindow(TRUE);
		m_btnLogout.EnableWindow(FALSE);
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(FALSE);
		m_btnExitRoom.EnableWindow(FALSE);
		m_btnStartDeviceTest.EnableWindow(FALSE);
		m_btnStopDeviceTest.EnableWindow(FALSE);
#endif
		ShowRetCode("StartContext", nError);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnStopContext(WPARAM wParam, LPARAM lParam)
{
	Error nError = (Error)wParam;
	if (AV_OK == nError)
	{
		m_sdkWrapper.DestroyContext();

		//释放音视频SDK后，退出登录
		TIMManager::get().Logout(&m_logoutCallBack);
		
		m_isLogin = false;
		m_isEnterRoom = false;

		m_btnEnableTestEnv.EnableWindow(TRUE);
		m_comboSceneTypeList.EnableWindow(TRUE);
		m_comboAccountList.EnableWindow(TRUE);
		m_comboChannelList.EnableWindow(TRUE);  //自己加的
		m_btnLogin.EnableWindow(TRUE);
		m_btnLogout.EnableWindow(FALSE);
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(FALSE);
		m_btnExitRoom.EnableWindow(FALSE);
		m_btnStartDeviceTest.EnableWindow(FALSE);
		m_btnStopDeviceTest.EnableWindow(FALSE);
#endif
	}
	else
	{
		m_btnEnableTestEnv.EnableWindow(TRUE);
		m_comboSceneTypeList.EnableWindow(TRUE);
		m_comboAccountList.EnableWindow(TRUE);
		m_comboChannelList.EnableWindow(TRUE);  //自己加的
		m_btnLogin.EnableWindow(TRUE);
		m_btnLogout.EnableWindow(FALSE);
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(FALSE);
		m_btnExitRoom.EnableWindow(FALSE);
		m_btnStartDeviceTest.EnableWindow(FALSE);
		m_btnStopDeviceTest.EnableWindow(FALSE);
#endif
		ShowRetCode("StopContext", nError);
	}

	return 0;
}

LONG DialogQAVSDKDemo::OnEnterRoom(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	if (AV_OK == retCode)
	{
		m_isEnterRoom = true;
		m_editRoomId.EnableWindow(FALSE);
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(FALSE);
		m_btnExitRoom.EnableWindow(TRUE);
		m_btnOpenCamera.EnableWindow(TRUE);
		m_btnCloseCamera.EnableWindow(TRUE);
		m_btnOpenMic.EnableWindow(TRUE);
		m_btnCloseMic.EnableWindow(TRUE);
		m_btnOpenPlayer.EnableWindow(TRUE);
		m_btnClosePlayer.EnableWindow(TRUE);
		m_btnOpenScreenShareSend.EnableWindow(TRUE);
		m_btnCloseScreenShareSend.EnableWindow(TRUE);

		m_btnCheckAudioDataMic.EnableWindow(TRUE);
		m_btnCheckAudioDataMixToSend.EnableWindow(TRUE);
		m_btnCheckAudioDataSend.EnableWindow(TRUE);
		m_btnCheckAudioDataNetStream.EnableWindow(TRUE);
		m_btnCheckAudioDataMixToPlay.EnableWindow(TRUE);
		m_btnCheckAudioDataPlay.EnableWindow(TRUE);
		m_sliderAudioDataVolume.EnableWindow(TRUE);
		m_comboAudioDataType.EnableWindow(TRUE);
		m_comboAudioDataSampleRate.EnableWindow(TRUE);
		m_comboAudioDataChannelNum.EnableWindow(TRUE);
		UpdateVideoPushBtnsState();
		m_btnCloseExternalCapture.EnableWindow(FALSE);
		m_btnOpenExternalCapture.EnableWindow(TRUE);
		m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif
		//创建房间成功后，打开摄像头、麦克风、扬声器
		int retCode = AV_OK;
		/*
		设置外部捕获的视频设备能力。
		注意：
		. 外部捕获跟内部摄像头是互斥的。
		. 如果设置了外部捕获设备的能力后，就不能再使用内部摄像头了；也就是，如果要使用内部摄像头，就不能设置外部捕获设备的能力。
		. 如果要使用外部捕获设备，不设置该能力也可以正常使用；当然，设置了，会给SDK提供更为准确的能力信息。
		. 如果要设置，必须在进入房间前就设置好。
		*/
	// 		CameraInfo stCamInfo;
	// 		stCamInfo.fps = 30;
	// 		stCamInfo.width = VIDEO_RENDER_BIG_VIEW_WIDTH;
	// 		stCamInfo.height = VIDEO_RENDER_BIG_VIEW_HEIGHT;
	// 		stCamInfo.device_id = "external_device_id";
	//		m_sdkWrapper.SetExternalCamAbility(&stCamInfo);

		m_comboScreenShareModeList.ResetContent();
		m_comboScreenShareModeList.SetWindowTextW(_T(""));
		m_comboScreenShareModeList.InsertString(0, _T("清晰模式"));
		m_comboScreenShareModeList.InsertString(1, _T("流畅模式"));
		m_comboScreenShareModeList.SetCurSel(0);

		m_viewLocalVideoRender.m_identifier = m_config.identifier;
		m_viewLocalVideoRender.m_videoSrcType = VIDEO_SRC_TYPE_CAMERA;
		retCode = m_sdkWrapper.OpenCamera();
 		ShowRetCode("OpenCamera", retCode);

		retCode = m_sdkWrapper.OpenMic();
		ShowRetCode("OpenMic", retCode);

		retCode = m_sdkWrapper.OpenPlayer();
		ShowRetCode("OpenPlayer", retCode);		

		m_micList.clear();
		m_playerList.clear();
		m_cameraList.clear();
		m_sdkWrapper.GetMicList(m_micList);
		m_sdkWrapper.GetPlayerList(m_playerList);
		m_sdkWrapper.GetCameraList(m_cameraList);

		m_comboMicList.ResetContent();
		m_comboMicList.SetWindowTextW(_T(""));
		if(m_micList.size() > 0)
		{
			for (int i = 0; i < m_micList.size(); i++)
			{
				//第一个设备是系统默认设备
				if(i == 0)m_comboMicList.InsertString(i, _T("(Windows默认设备)") + CString(m_micList[i].second.c_str()));
				else m_comboMicList.InsertString(i, CString(m_micList[i].second.c_str()));      
			}
			m_comboMicList.SetCurSel(0);
		}
    
		m_comboPlayerList.ResetContent();
		m_comboPlayerList.SetWindowTextW(_T(""));
		if(m_playerList.size() > 0)
		{
			for (int i = 0; i < m_playerList.size(); i++)
			{
				//第一个设备是系统默认设备
				if(i == 0)m_comboPlayerList.InsertString(i, _T("(Windows默认设备)") + CString(m_playerList[i].second.c_str()));
				else m_comboPlayerList.InsertString(i, CString(m_playerList[i].second.c_str()));  
			}
			m_comboPlayerList.SetCurSel(0);
		}

		m_comboCameraList.ResetContent();
		m_comboCameraList.SetWindowTextW(_T(""));
		if(m_cameraList.size() > 0)
		{
			for (int i = 0; i < m_cameraList.size(); i++)
			{
				m_comboCameraList.InsertString(i, CString(m_cameraList[i].second.c_str()));      
			}
			m_comboCameraList.SetCurSel(0);
		}
		m_btnProtoAuth.EnableWindow(TRUE);
		m_editProtoAuthBuffer.EnableWindow(TRUE);
		
		UpdateAudioDataState();
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnEnterRoom.EnableWindow(TRUE);
		m_btnExitRoom.EnableWindow(FALSE);
		m_btnOpenCamera.EnableWindow(FALSE);
		m_btnCloseCamera.EnableWindow(FALSE);
		m_btnOpenMic.EnableWindow(FALSE);
		m_btnCloseMic.EnableWindow(FALSE);
		m_btnOpenPlayer.EnableWindow(FALSE);
		m_btnClosePlayer.EnableWindow(FALSE);
		m_btnOpenScreenShareSend.EnableWindow(FALSE);
		m_btnCloseScreenShareSend.EnableWindow(FALSE);

		m_btnCheckAudioDataMic.EnableWindow(FALSE);
		m_btnCheckAudioDataMixToSend.EnableWindow(FALSE);
		m_btnCheckAudioDataSend.EnableWindow(FALSE);
		m_btnCheckAudioDataNetStream.EnableWindow(FALSE);
		m_btnCheckAudioDataMixToPlay.EnableWindow(FALSE);
		m_btnCheckAudioDataPlay.EnableWindow(FALSE);
		m_sliderAudioDataVolume.EnableWindow(FALSE);
		m_comboAudioDataType.EnableWindow(FALSE);
		m_comboAudioDataSampleRate.EnableWindow(FALSE);
		m_comboAudioDataChannelNum.EnableWindow(FALSE);

		m_btnOpenExternalCapture.EnableWindow(FALSE);
		m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif		
		ShowRetCode("EnterRoom", retCode);
	}
	
	return 0;
}

LONG DialogQAVSDKDemo::OnExitRoom(WPARAM wParam, LPARAM lParam)
{
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnEnterRoom.EnableWindow(TRUE);
	m_btnExitRoom.EnableWindow(FALSE);
	m_btnOpenCamera.EnableWindow(FALSE);
	m_btnCloseCamera.EnableWindow(FALSE);
	m_btnOpenMic.EnableWindow(FALSE);
	m_btnCloseMic.EnableWindow(FALSE);
	m_btnOpenPlayer.EnableWindow(FALSE);
	m_btnClosePlayer.EnableWindow(FALSE);
	m_btnOpenScreenShareSend.EnableWindow(FALSE);
	m_btnCloseScreenShareSend.EnableWindow(FALSE);

	m_btnCheckAudioDataMic.EnableWindow(FALSE);
	m_btnCheckAudioDataMixToSend.EnableWindow(FALSE);
	m_btnCheckAudioDataSend.EnableWindow(FALSE);
	m_btnCheckAudioDataNetStream.EnableWindow(FALSE);
	m_btnCheckAudioDataMixToPlay.EnableWindow(FALSE);
	m_btnCheckAudioDataPlay.EnableWindow(FALSE);
	m_sliderAudioDataVolume.EnableWindow(FALSE);
	m_comboAudioDataType.EnableWindow(FALSE);
	m_comboAudioDataSampleRate.EnableWindow(FALSE);
	m_comboAudioDataChannelNum.EnableWindow(FALSE);

	m_btnOpenExternalCapture.EnableWindow(FALSE);
	m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif
	m_isEnterRoom = false;
	m_editRoomId.EnableWindow(TRUE);

	m_viewLocalVideoRender.Clear();
	m_viewBigVideoRender.Clear();
	m_viewScreenShareRender.Clear();

	m_listEndpointList.ResetContent();	
	m_listEndpointList.SetWindowTextW(_T(""));
	m_micList.clear();
	m_playerList.clear();
	m_cameraList.clear();
	m_comboCameraList.ResetContent();//先清空
	m_comboCameraList.SetWindowTextW(_T(""));
	m_comboMicList.ResetContent();//先清空
	m_comboMicList.SetWindowTextW(_T(""));
	m_comboPlayerList.ResetContent();//先清空
	m_comboPlayerList.SetWindowTextW(_T(""));
		
	int retCode = (int)wParam;
	ShowRetCode("ExitRoom. ", retCode);
	if(m_isExitDemo)m_sdkWrapper.StopContext();

  	m_bEnableExCap = FALSE;

  	m_btnProtoAuth.EnableWindow(FALSE);
  	m_btnProtoAuth.SetWindowText(L"修改权限");

	UpdateVideoPushBtnsState();
	return 0;
}

LONG DialogQAVSDKDemo::OnEnpointsUpdateInfo(WPARAM wParam, LPARAM lParam)
{
	UpdateEndpointList();
	UpadateVideoRenderInfo();
	return 0;
}

LONG DialogQAVSDKDemo::OnRequestViewList(WPARAM wParam, LPARAM lParam)
{
	UpadateVideoRenderInfo();

	Error retCode = (Error)wParam;
	if (AV_OK == retCode)
	{

	}
	else
	{
		ShowRetCode("RequestViewList. ", retCode);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnCancelAllView(WPARAM wParam, LPARAM lParam)
{
	UpadateVideoRenderInfo();

	Error retCode = (Error)wParam;
	if (AV_OK == retCode)
	{

	}
	else
	{
		ShowRetCode("CancelAllView.  ", retCode);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnMicOperation(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			uint32 micVolume = m_sdkWrapper.GetMicVolume();
			CString strMicVolume = _T("");
			strMicVolume.Format(_T("%d"), micVolume);
			m_sliderAudioMicVolume.SetPos(micVolume);
			m_staticMicCurVolume.SetWindowTextW(strMicVolume);
			m_isEnableMic = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenMic.EnableWindow(FALSE);
			m_btnCloseMic.EnableWindow(TRUE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnableMic = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenMic.EnableWindow(TRUE);
			m_btnCloseMic.EnableWindow(FALSE);
#endif
			ShowRetCode("MicOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{	
			uint32 micVolume = 0;
			CString strMicVolume = _T("");
			strMicVolume.Format(_T("%d"), micVolume);
			m_sliderAudioMicVolume.SetPos(micVolume);
			m_staticMicCurVolume.SetWindowTextW(strMicVolume);
			m_isEnableMic = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenMic.EnableWindow(TRUE);
			m_btnCloseMic.EnableWindow(FALSE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnableMic = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenMic.EnableWindow(FALSE);
			m_btnCloseMic.EnableWindow(TRUE);
#endif
			ShowRetCode("MicOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("Mic", retCode);
	return 0;
}

LONG DialogQAVSDKDemo::OnPlayerOperation(WPARAM wParam, LPARAM lParam)
{	
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			uint32 playerVolume = m_sdkWrapper.GetPlayerVolume();
			CString strPlayerVolume = _T("");
			strPlayerVolume.Format(_T("%d"), playerVolume);
			m_sliderAudioPlayerVolume.SetPos(playerVolume);
			m_staticPlayerCurVolume.SetWindowTextW(strPlayerVolume);
			m_isEnablePlayer = true;	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenPlayer.EnableWindow(FALSE);
			m_btnClosePlayer.EnableWindow(TRUE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnablePlayer = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenPlayer.EnableWindow(TRUE);
			m_btnClosePlayer.EnableWindow(FALSE);
#endif
			ShowRetCode("PlayerOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{	
			uint32 playerVolume = 0;
			CString strPlayerVolume = _T("");
			strPlayerVolume.Format(_T("%d"), playerVolume);
			m_sliderAudioPlayerVolume.SetPos(playerVolume);
			m_staticPlayerCurVolume.SetWindowTextW(strPlayerVolume);
			m_isEnablePlayer = false;	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenPlayer.EnableWindow(TRUE);
			m_btnClosePlayer.EnableWindow(FALSE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnablePlayer = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenPlayer.EnableWindow(FALSE);
			m_btnClosePlayer.EnableWindow(TRUE);
#endif
			ShowRetCode("PlayerOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("Player", retCode);	
	return 0;
}

LONG DialogQAVSDKDemo::OnAccompanyOperation(WPARAM wParam, LPARAM lParam)
{	
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			uint32 accompanyVolume = m_sdkWrapper.GetAccompanyVolume();
			CString strAccompanyVolume = _T("");
			strAccompanyVolume.Format(_T("%d"), accompanyVolume);
			m_sliderAccompanyVolume.SetPos(accompanyVolume);
			m_staticAcompanyCurVolume.SetWindowTextW(strAccompanyVolume);
		}
		else if (AV_ERR_FAILED == retCode)
		{
			ShowRetCode("AccompanyOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{	
			uint32 accompanyVolume = 0;
			CString strAccompanyVolume = _T("");
			strAccompanyVolume.Format(_T("%d"), accompanyVolume);
			m_sliderAccompanyVolume.SetPos(accompanyVolume);
			m_staticAcompanyCurVolume.SetWindowTextW(strAccompanyVolume);
		}
		else if (AV_ERR_FAILED == retCode)
		{
			ShowRetCode("AccompanyOperation. ", retCode);
		}
	}
	
	return 0;
}

LONG DialogQAVSDKDemo::OnCameraOperation(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{		
		if (AV_OK == retCode)
		{	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenCamera.EnableWindow(FALSE);
			m_btnCloseCamera.EnableWindow(TRUE);
#endif
			m_isEnableCamera = true;
			m_viewLocalVideoRender.m_identifier = m_config.identifier;
			m_viewLocalVideoRender.m_videoSrcType = VIDEO_SRC_TYPE_CAMERA;
		}
		else if (AV_ERR_FAILED == retCode)
		{
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenCamera.EnableWindow(TRUE);
			m_btnCloseCamera.EnableWindow(FALSE);
#endif
			m_isEnableCamera = false;
			m_viewLocalVideoRender.m_identifier = "";
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{		
		//TODO
		if (AV_OK == retCode)
		{	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenCamera.EnableWindow(TRUE);
			m_btnCloseCamera.EnableWindow(FALSE);
#endif
			m_isEnableCamera = false;
		}
		else if (AV_ERR_FAILED == retCode)
		{
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenCamera.EnableWindow(FALSE);
			m_btnCloseCamera.EnableWindow(TRUE);
#endif
			m_isEnableCamera = true;
		}
	}
	//if(retCode != AV_OK)ShowRetCode("Camera", retCode);
	return 0;
}


LONG DialogQAVSDKDemo::OnScreenShareSendOperation(WPARAM wParam, LPARAM lParam)
{	
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{			
			m_isEnableScreenShareSend = true;	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenScreenShareSend.EnableWindow(FALSE);
			m_btnCloseScreenShareSend.EnableWindow(TRUE);
			m_editScreenShareCaptureLeft.EnableWindow(FALSE);
			m_editScreenShareCaptureTop.EnableWindow(FALSE);
			m_editScreenShareCaptureRight.EnableWindow(FALSE);
			m_editScreenShareCaptureBottom.EnableWindow(FALSE);
			m_editScreenShareCaptureFps.EnableWindow(FALSE);
#endif			
			m_viewScreenShareRender.m_identifier = m_config.identifier;
			m_viewScreenShareRender.m_videoSrcType = VIDEO_SRC_TYPE_SCREEN;
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnableScreenShareSend = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenScreenShareSend.EnableWindow(TRUE);
			m_btnCloseScreenShareSend.EnableWindow(FALSE);
			m_editScreenShareCaptureLeft.EnableWindow(TRUE);
			m_editScreenShareCaptureTop.EnableWindow(TRUE);
			m_editScreenShareCaptureRight.EnableWindow(TRUE);
			m_editScreenShareCaptureBottom.EnableWindow(TRUE);
			m_editScreenShareCaptureFps.EnableWindow(TRUE);
#endif
			m_viewScreenShareRender.m_identifier = "";
			ShowRetCode("ScreenShareSendOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{
			m_isEnableScreenShareSend = false;	
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenScreenShareSend.EnableWindow(TRUE);
			m_btnCloseScreenShareSend.EnableWindow(FALSE);
			m_editScreenShareCaptureLeft.EnableWindow(TRUE);
			m_editScreenShareCaptureTop.EnableWindow(TRUE);
			m_editScreenShareCaptureRight.EnableWindow(TRUE);
			m_editScreenShareCaptureBottom.EnableWindow(TRUE);
			m_editScreenShareCaptureFps.EnableWindow(TRUE);
#endif
			m_viewScreenShareRender.m_identifier = "";
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isEnableScreenShareSend = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnOpenScreenShareSend.EnableWindow(FALSE);
			m_btnCloseScreenShareSend.EnableWindow(TRUE);
			m_editScreenShareCaptureLeft.EnableWindow(FALSE);
			m_editScreenShareCaptureTop.EnableWindow(FALSE);
			m_editScreenShareCaptureRight.EnableWindow(FALSE);
			m_editScreenShareCaptureBottom.EnableWindow(FALSE);
			m_editScreenShareCaptureFps.EnableWindow(FALSE);
#endif
			m_viewScreenShareRender.m_identifier = "";
			ShowRetCode("ScreenShareSendOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("ScreenShareSend", retCode);	
	return 0;
}


LONG DialogQAVSDKDemo::OnEndpointMenuItem(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)return FALSE;

	CString* pStrOpenid = (CString*)wParam;
	listCommand cmd = (listCommand)lParam;

	string identifier = StrWToStrA(*pStrOpenid);

	if(m_config.identifier == identifier)
	{
		ShowMessageBox(_T("不能对自己进行该操作!"));
		return 0;
	}
	
	AVEndpoint *endpoint = m_sdkWrapper.GetEndpointById(identifier);
	if(endpoint == NULL
		|| (cmd == LISTCOMMAND_REQEUST_CAMERA_VIEW && !endpoint->HasCameraVideo())
		|| (cmd == LISTCOMMAND_REQEUST_SCREEN_VIEW && !endpoint->HasScreenVideo()))
	{
		ShowMessageBox(_T("该成员不存在这种视频源类型的画面，不能请求!"));
		return 0;
	}

	std::vector<std::string> identifierList = m_sdkWrapper.GetRequestViewIdentifierList();
	std::vector<View> viewList = m_sdkWrapper.GetRequestViewParamList();

	if(cmd == LISTCOMMAND_CANCEL_CAMERA_VIEW || cmd == LISTCOMMAND_CANCEL_SCREEN_VIEW)
	{
		int cancelingVideoSrcType = VIDEO_SRC_TYPE_NONE;
		if(cmd == LISTCOMMAND_CANCEL_CAMERA_VIEW)cancelingVideoSrcType = VIDEO_SRC_TYPE_CAMERA;
		else if(cmd == LISTCOMMAND_CANCEL_SCREEN_VIEW)cancelingVideoSrcType = VIDEO_SRC_TYPE_SCREEN;

		bool hasRequest = false;
		for(int i = 0; i < identifierList.size(); i++)
		{
			//注意：这边DEMO做得比较简单，不考虑大小画面的情况；实际应用中，是要考虑大小画面的，需要判断请求的画面大小是否不变。
			if(identifierList[i] == identifier && viewList[i].video_src_type == cancelingVideoSrcType)
			{				
				hasRequest = true;
				break;
			}
		}
		
		if(!hasRequest)
		{
			ShowMessageBox(_T("未请求该成员这种视频源类型的画面，不需要取消!"));
			return 0;
		}
	}

	if(cmd == LISTCOMMAND_REQEUST_CAMERA_VIEW || cmd == LISTCOMMAND_REQEUST_SCREEN_VIEW)
	{	
		VideoSrcType videoSrcType = (cmd == LISTCOMMAND_REQEUST_CAMERA_VIEW ? VIDEO_SRC_TYPE_CAMERA : VIDEO_SRC_TYPE_SCREEN);
		int i = 0;
		for (; i < identifierList.size(); i++)
		{
			if (identifierList[i] == identifier && viewList[i].video_src_type == videoSrcType)
			{
				break;//重复
			}
		}

		if (i < identifierList.size())
		{
			ShowMessageBox(_T("重复请求这个人的画面!"));
			return 0;
		}
		else
		{
			identifierList.push_back(identifier);
			View view;
			view.size_type = VIEW_SIZE_TYPE_BIG;//这边，DEMO默认请求大画面；实际业务要请求大画面还是小画面，可以根据使用场景决定。
			view.video_src_type = videoSrcType;
			viewList.push_back(view);

			int retCode = m_sdkWrapper.RequestViewList(identifierList, viewList);
			if(retCode != AV_OK)
			{
				int i = 0;
				for (; i < identifierList.size(); i++)
				{
					if (identifierList[i] == identifier && viewList[i].video_src_type == videoSrcType)
					{
						identifierList.erase(identifierList.begin() + i);
						viewList.erase(viewList.begin() + i);
						break;//清除请求失败的id
					}
				}
				ClearOperationTips();
			}
			ShowRetCode("RequestViewList", retCode);
		}
	}
	else if (cmd == LISTCOMMAND_CANCEL_CAMERA_VIEW || cmd == LISTCOMMAND_CANCEL_SCREEN_VIEW)
	{
		VideoSrcType videoSrcType = (cmd == LISTCOMMAND_CANCEL_CAMERA_VIEW ? VIDEO_SRC_TYPE_CAMERA : VIDEO_SRC_TYPE_SCREEN);

		int i = 0;
		for (; i < identifierList.size(); i++)
		{
			if (identifierList[i] == identifier && viewList[i].video_src_type == videoSrcType)
			{
				break;//存在
			}
		}

		if (i == identifierList.size())
		{
			ShowMessageBox(_T("没有请求这个人的画面，不需要取消!"));
			return 0;
		}
		else
		{
			if (identifierList.size() == 0)
			{
				ShowMessageBox(_T("没有需要取消的画面!"));
				return 0;
			}

			int i = 0;
			for (; i < identifierList.size(); i++)
			{
				if (identifierList[i] == identifier && viewList[i].video_src_type == videoSrcType)
				{
					identifierList.erase(identifierList.begin() + i);
					viewList.erase(viewList.begin() + i);
					break;//清除要取消画面的id
				}
			}

			int retCode = AV_OK;
			if(identifierList.size() > 0)
			{
				retCode = m_sdkWrapper.RequestViewList(identifierList, viewList);
				if(retCode != AV_OK)
				{					
					ClearOperationTips();					
				}
			}
			else
			{
				identifierList.clear();
				viewList.clear();
				retCode = m_sdkWrapper.CancelAllView();
				if(retCode != AV_OK)ClearOperationTips();
				
			}
			
			ShowRetCode("CancelAllView", retCode);
		}		
	}
	else if (cmd == LISTCOMMAND_NOT_MUTE_AUDIO)
	{
		int retCode = m_sdkWrapper.MuteAudio(identifier, false);
		ShowRetCode("MuteAudio", retCode);
	}
	else if (cmd == LISTCOMMAND_MUTE_AUDIO)
	{
		int retCode = m_sdkWrapper.MuteAudio(identifier, true);
		ShowRetCode("MuteAudio", retCode);
	}

	return 0;
}


void DialogQAVSDKDemo::OnBnClickedBtnLogin()
{
	//开启crash上报
	m_sdkWrapper.EnableCrashReport(true);

	if(m_isLogin)
	{
		ShowMessageBox(_T("已经登录！"));
		return;
	}

	if(m_editRoomId.GetWindowTextLengthW() == 0)
	{
		ShowMessageBox(_T("房间号不能为空！"));
		return;
	}  
	

	//切换后台环境, 1表示测试环境，0表示正式环境
	TIMManager::get().set_env(m_isTestEnv ? 1 : 0);
	
	//设置模式。如果要支持日志上报功能，需要将模式设置为"1"。
	TIMManager::get().set_mode(1); 

	//先初始化通信SDK
	TIMManager::get().Init();
	m_btnEnableTestEnv.EnableWindow(FALSE);

	SetMainHWnd(this->m_hWnd);

	CString cloudSdkAppId = _T("");

	//腾讯为每个接入方分配的账号类型
	CString accountType = _T("");

	//App使用的OAuth授权体系分配的AppId
	CString appIdAt3rd = _T("");

	//用户账号
	CString identifier = _T("");

	//用户密码
	CString userSig = _T("");

	//频道名称，自己加的
	CString channelName = _T("");

	//频道描述，自己加的
	CString channelDescription = _T("");

	//腾讯为每个使用SDK的App分配的AppId
	CString sdkAppId = _T("");

	m_editAccountType.GetWindowTextW(accountType);
	m_editAppIdAt3rd.GetWindowTextW(appIdAt3rd);
	m_editSdkAppId.GetWindowTextW(sdkAppId);
	m_comboAccountList.GetWindowTextW(identifier);
	m_comboChannelList.GetWindowTextW(channelName);  //自己加的

	if (identifier.GetLength() > 0)
	{
		AccountInfo info = ConfigInfoMgr::GetInst()->GetAccountInfoById(identifier);
		userSig = info.userSig;
	}
	else
	{
		ShowMessageBox(_T("账号不能为空！"));
		return;
	}

	if (accountType.GetLength() > 0 && appIdAt3rd.GetLength() > 0
		&& identifier.GetLength() > 0 && userSig.GetLength() > 0
		&& sdkAppId.GetLength() > 0)
	{

		m_config.account_type = StrWToStrA(accountType);
		m_config.app_id_at3rd = StrWToStrA(appIdAt3rd);
		m_config.sdk_app_id = _ttoi(sdkAppId);
		m_config.identifier = StrWToStrA(identifier);
		m_userSig = StrWToStrA(userSig);


		//账号登录部分
		//填写登录使用的用户信息
		TIMUser user;
		user.set_account_type(m_config.account_type);
		user.set_app_id_at_3rd(m_config.app_id_at3rd);
		user.set_identifier(m_config.identifier);
		//初始化完通信SDK后，开始登录
		ShowOperationTips(m_operationTipsLogining, 1000);

		//参数解释（腾讯为每个使用SDK的App分配的AppId，用户信息，用户密码，登录成功和失败的回调方法）
		TIMManager::get().Login(1400007961, user, "eJx1jkFvgjAARu-8iqZnI0VYYbshEcMEHYM45dIw20plw0prxrbsvy8yE7ns-F6*930bAACYx9m43O2O50YT-SkZBA8AOgghOLpxKQUlpSZ2S3tuXQTk3mNrYLFOipaRkmvW-ll3LvYu4nBLUNZowcXVeesGTNGa9K3-I0rse5jMtkGUBs2iMKs047OwM2MpTyeeRyzEy*zLaw-BclP5r26uVyqcfkSVHy8ek0NGdbL3az53ttT2oiJdYVm4U6VMXp*PaGI-r82nl0FSi3d2PYQtbDuWM4HGj-ELU2ZWgg__", &m_loginCallBack/*m_config.sdk_app_id, user, m_userSig, &m_loginCallBack*/);
	}
	else
	{
		ShowMessageBox(_T("APP配置信息不能为空！"));
		return;
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnLogout()
{
	if(!m_isLogin)
	{
		ShowMessageBox(_T("没有登录！"));
		return;
	}

	if(m_isEnterRoom)
	{
		//ShowMessageBox(_T("还在房间中，请先退出房间再退出登录！"));
		//return;
	}

	//先释放音视频SDK
	ShowOperationTips(m_operationTipsLogouting, 500);
	int retCode = m_sdkWrapper.StopContext();	
	if(retCode != AV_OK)ClearOperationTips();
	ShowRetCode("StopContext", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnEnterRoom()
{
	if(!m_isLogin)
	{
		ShowMessageBox(_T("没有登录，不能进入房间！"));
		return;
	}

	if(m_isStartDeviceTest)
	{
		ShowMessageBox(_T("设备测试中，不能进入房间。请先停止测试再进入房间！"));
		return;
	}

	//获取屏幕画面大小
	HWND hwndDesktop = ::GetDesktopWindow();  //返回桌面窗口的句柄。桌面窗口覆盖整个屏幕。桌面窗口是一个要在其上绘制所有的图标和其他窗口的区域
	RECT rc;
	::GetWindowRect(hwndDesktop, &rc);  //返回指定窗口的边框矩形的尺寸给rc
	
	CString strLeft = _T("");
	strLeft.Format(_T("%d"), (int)rc.left);
	m_editScreenShareCaptureLeft.SetWindowTextW(strLeft);

	CString strTop = _T("");
	strTop.Format(_T("%d"), (int)rc.top);
	m_editScreenShareCaptureTop.SetWindowTextW(strTop);

	CString strRight = _T("");
	strRight.Format(_T("%d"), (int)rc.right);
	m_editScreenShareCaptureRight.SetWindowTextW(strRight);

	CString strBottom = _T("");
	strBottom.Format(_T("%d"), (int)rc.bottom);	
	m_editScreenShareCaptureBottom.SetWindowTextW(strBottom);	

	CString strFps = _T("");
	strFps.Format(_T("%d"), (int)DEFAULT_SCREEN_VIDEO_CAPTURE_FPS);	
	m_editScreenShareCaptureFps.SetWindowTextW(strFps);
			
	CString strRoomIdTemp;
	m_editRoomId.GetWindowTextW(strRoomIdTemp);

	uint32 roomId = _ttoi(strRoomIdTemp);

  /*
	if(m_isTestEnv)
	{
		if(roomId < 200001 || roomId > 200020)
		{
			ShowMessageBox(_T("测试环境下讨论组号只能取200001-200020之间！\r\n测试同事请用：200001-200005\r\n开发同事请用：200006-200010\r\n其他人请用 ：200011-200015\r\n以下暂时保留：200016-200020\r\n"));
			return;
		}
	}
  */

	if(roomId < 1 || roomId > 4294967295)
	{
		ShowMessageBox(_T("房间号必须在1-4294967295之间！"));
		return;
	}

	CString strRoomRoleTemp;
	m_editRoomRole.GetWindowTextW(strRoomRoleTemp);
	std::string avControlRole = StrWToStrA(strRoomRoleTemp);

	AVRoomMulti::EnterRoomParam enterRoomParam;
	enterRoomParam.app_room_id = roomId;//这是业务层自己的房间号，可以填业务层的讨论组号/群号/座号/房间号等，只要具有唯一性即可。		
	enterRoomParam.av_control_role = avControlRole;

	if( 0 != m_btnCheckHasVideoSendAuth.GetCheck() && 
		0 != m_btnCheckHasAudioSendAuth.GetCheck() && 
		0 != m_btnCheckHasVideoRevAuth.GetCheck() && 
		0 != m_btnCheckHasAudioRevAuth.GetCheck() )
	{
		enterRoomParam.auth_bits = AUTH_BITS_DEFUALT; //权限位；默认值是拥有所有权限。TODO：请业务侧填根据自己的情况填上权限位。	
	}
	else
	{
		enterRoomParam.auth_bits = AUTH_BITS_CREATE_ROOM | AUTH_BITS_JOIN_ROOM | AUTH_BITS_RECV_SCREEN_VIDEO;
		if( 0 != m_btnCheckHasVideoSendAuth.GetCheck() )
			enterRoomParam.auth_bits |= AUTH_BITS_SEND_CAMERA_VIDEO;
		if( 0 != m_btnCheckHasAudioSendAuth.GetCheck() )
			enterRoomParam.auth_bits |= AUTH_BITS_SEND_AUDIO;
		if( 0 != m_btnCheckHasVideoRevAuth.GetCheck() )
			enterRoomParam.auth_bits |= AUTH_BITS_RECV_CAMERA_VIDEO;
		if( 0 != m_btnCheckHasAudioRevAuth.GetCheck() )
			enterRoomParam.auth_bits |= AUTH_BITS_RECV_AUDIO;
	}

	//设置音视频编解码参数。如果需要设定特定的音视频参数，请在创建房间前设置好。
	//主播场景，支持设置自定义音视频参数。
	if((m_config.account_type == "107" && m_config.app_id_at3rd == "1104062745" && m_config.sdk_app_id == 1104062745))
	{
		//音频场景策略；有三种取值：实时通信场景，直播场景中的主播人员，直播场景中的听众；TODO：请业务侧根据自己的情况填这个值。
		enterRoomParam.audio_category = AVRoom::AUDIO_CATEGORY_MEDIA_PLAY_AND_RECORD;//直播场景
	}
	else
	{
		//音频场景策略；有三种取值：实时通信场景，直播场景中的主播人员，直播场景中的听众；TODO：请业务侧根据自己的情况填这个值。
		enterRoomParam.audio_category = AVRoom::AUDIO_CATEGORY_VOICECHAT;//实时通信场景
	}

	enterRoomParam.auto_create_room = (m_chkIsAutoCreateSDKRoom.GetCheck() == 1);
  
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnEnterRoom.EnableWindow(FALSE);
#endif
	ShowOperationTips(m_operationTipsEnterRooming, 3000);
	int retCode = m_sdkWrapper.EnterRoom(enterRoomParam);
	if(retCode != AV_OK)ClearOperationTips();
#ifdef ENABLE_UI_OPERATION_SAFETY
	if(retCode != AV_OK)m_btnEnterRoom.EnableWindow(TRUE);
#endif
	ShowRetCode("EnterRoom", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnExitRoom()
{	
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	int retCode = AV_OK;
// 	retCode = m_sdkWrapper.CloseCamera();
// 	//ShowRetCode("CloseCamera", retCode);
// 	retCode = m_sdkWrapper.CloseMic();
// 	//ShowRetCode("CloseCamera", retCode);
// 	retCode = m_sdkWrapper.ClosePlayer();
// 	//ShowRetCode("CloseCamera", retCode);

	m_isEnableMic = false;
	m_isEnablePlayer = false;
	m_isEnableScreenShareSend = false;
	m_isEnableCamera = false;

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnExitRoom.EnableWindow(FALSE);
#endif
	ShowOperationTips(m_operationTipsExitRooming, 500);
	retCode = m_sdkWrapper.ExitRoom();
	if(retCode != AV_OK)ClearOperationTips();
#ifdef ENABLE_UI_OPERATION_SAFETY
	if(retCode != AV_OK)m_btnExitRoom.EnableWindow(TRUE);
#endif
	ShowRetCode("ExitRoom", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnAddAccount()
{
	DialogAddAccount dlg;
	if(dlg.DoModal() == IDOK)  //显示一个模态对话框，此时DoModal()函数并不返回，直到用户关闭此对话框时，DoModal才返回。如果用户点击了确定按钮，那么DoModal()函数就返回IDOK，IDOK是一个宏
	{
		UpdateConfigInfo();
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnAddChannel()  //自己加的
{
	DialogAddChannel dlg;
	if(dlg.DoModal() == IDOK)  //显示一个模态对话框，此时DoModal()函数并不返回，直到用户关闭此对话框时，DoModal才返回。如果用户点击了确定按钮，那么DoModal()函数就返回IDOK，IDOK是一个宏
	{
		UpdateConfigInfo();
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnDeleteAccount()
{
	CString identifier;
	m_comboAccountList.GetWindowTextW(identifier);  
	m_comboAccountList.ResetContent();
	m_comboAccountList.SetWindowTextW(_T(""));//先清空
	ConfigInfoMgr::GetInst()->RemoveAccount(identifier);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	UpdateConfigInfo();
}

void DialogQAVSDKDemo::OnBnClickedBtnDeleteChannel()  //自己加的
{
	CString channelName;
	m_comboChannelList.GetWindowTextW(channelName);  
	m_comboChannelList.ResetContent();
	m_comboChannelList.SetWindowTextW(_T(""));//先清空
	ConfigInfoMgr::GetInst()->RemoveAccount(channelName);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	UpdateConfigInfo();
}

void DialogQAVSDKDemo::OnBnClickedBtnAddApp()
{
	DialogAddAppConfig dlg;
	if(dlg.DoModal() == IDOK)
	{
		UpdateConfigInfo();
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnDeleteApp()
{
	CString sceneType;
	m_comboSceneTypeList.GetWindowTextW(sceneType);
	m_comboSceneTypeList.ResetContent();//先清空
	m_comboSceneTypeList.SetWindowTextW(_T(""));
	m_editAccountType.SetWindowTextW(_T(""));
	m_editAppIdAt3rd.SetWindowTextW(_T(""));
	m_editSdkAppId.SetWindowTextW(_T(""));	
	ConfigInfoMgr::GetInst()->RemoveApp(sceneType);
	ConfigInfoMgr::GetInst()->SaveConfigInfo();
	UpdateConfigInfo();
}

void DialogQAVSDKDemo::OnBnClickedBtnCheckYuvOutput()
{
	if(m_pYuvOutputFile)
	{
		fclose(m_pYuvOutputFile);
		m_pYuvOutputFile = NULL;
	}

	AVSupportVideoPreview::PreviewParam param;
	param.device_id = "";
	param.width = VIDEO_RENDER_BIG_VIEW_WIDTH;
	param.height = VIDEO_RENDER_BIG_VIEW_HEIGHT;
	param.src_type = VIDEO_SRC_TYPE_CAMERA;

	if( 0 == m_btnCheckYuvOutput.GetCheck() )
	{
		param.color_format = COLOR_FORMAT_RGB24;
		m_sdkWrapper.SetVideoParam(param);
	}
	else
	{
		m_sdkWrapper.ClearVideoParam();
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataMic()
{
	if( 0 != m_btnCheckAudioDataMic.GetCheck() )
		m_sdkWrapper.StartOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIC);
	else
		m_sdkWrapper.StopOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIC);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_MIC);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataMixToSend()
{
	CString cstrMixToSendPath; 
  m_editMixToSendPath.GetWindowText(cstrMixToSendPath);
  std::string strMixToSendPath = StrWToStrA(cstrMixToSendPath);

	if( 0 != m_btnCheckAudioDataMixToSend.GetCheck() )
		m_sdkWrapper.StartInputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND, strMixToSendPath);
	else
		m_sdkWrapper.StopInputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataSend()
{
	if( 0 != m_btnCheckAudioDataSend.GetCheck() )
		m_sdkWrapper.StartOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_SEND);
	else
		m_sdkWrapper.StopOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_SEND);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_SEND);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataNetStream()
{
	if( 0 != m_btnCheckAudioDataNetStream.GetCheck() )
		m_sdkWrapper.StartOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM);
	else
		m_sdkWrapper.StopOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataMixToPlay()
{
	CString cstrMixToPlayPath; 
  m_editMixToPlayPath.GetWindowText(cstrMixToPlayPath);
  std::string strMixToPlayPath = StrWToStrA(cstrMixToPlayPath);

	if( 0 != m_btnCheckAudioDataMixToPlay.GetCheck() )
		m_sdkWrapper.StartInputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY, strMixToPlayPath);
	else
		m_sdkWrapper.StopInputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnAudioDataPlay()
{
	if( 0 != m_btnCheckAudioDataPlay.GetCheck() )
		m_sdkWrapper.StartOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY);
	else
		m_sdkWrapper.StopOutputAudioDataProcess(AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY);

	m_comboAudioDataType.SetCurSel(AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY);
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnCbnSelchangeComboAudioDataType()
{
	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnCbnSelchangeComboAudioDataParam()
{
	AVAudioCtrl::AudioDataSourceType type = (AVAudioCtrl::AudioDataSourceType)m_comboAudioDataType.GetCurSel();
	AudioFrameDesc audio_desc;
	audio_desc.sample_rate = g_AudioDataSampleRates[ m_comboAudioDataSampleRate.GetCurSel() ].value;
	audio_desc.channel_num = g_AudioDataChannelNumStates[ m_comboAudioDataChannelNum.GetCurSel() ].value;
	audio_desc.bits = 16;

	m_sdkWrapper.SetAudioDataFormat(type, audio_desc);

	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnChangeAudioDataVolume()
{
	AVAudioCtrl::AudioDataSourceType type = (AVAudioCtrl::AudioDataSourceType)m_comboAudioDataType.GetCurSel();
	int volume = m_sliderAudioDataVolume.GetPos();  
	if (volume < MIN_AUDIO_DEVICE_VOLUME)
		volume = MIN_AUDIO_DEVICE_VOLUME;
	else if (volume > MAX_AUDIO_DEVICE_VOLUME)
		volume = MAX_AUDIO_DEVICE_VOLUME;

	m_sdkWrapper.SetAudioDataVolume(type, (float)volume / (float)MAX_AUDIO_DEVICE_VOLUME );

	UpdateAudioDataState();
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenCamera()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenCamera.EnableWindow(FALSE);
	m_btnCloseCamera.EnableWindow(TRUE);
#endif
	ShowOperationTips(m_operationTipsOpenCameraing, 1000);
	m_isEnableCamera = true;

	m_viewLocalVideoRender.m_identifier = m_config.identifier;
	m_viewLocalVideoRender.m_videoSrcType = VIDEO_SRC_TYPE_CAMERA;

	int retCode = m_sdkWrapper.OpenCamera();	
	if(retCode != AV_OK)ClearOperationTips();//ulqejqfoorslbcff
	if(retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenCamera.EnableWindow(TRUE);
		m_btnCloseCamera.EnableWindow(FALSE);
#endif
		m_isEnableCamera = false;
		m_viewLocalVideoRender.m_identifier = "";
	}

	ShowRetCode("OpenCamera", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnCloseCamera()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenCamera.EnableWindow(TRUE);
	m_btnCloseCamera.EnableWindow(FALSE);
#endif
	m_viewLocalVideoRender.Clear();

	if(m_viewBigVideoRender.m_identifier == m_config.identifier 
		&& m_viewBigVideoRender.m_videoSrcType == VIDEO_SRC_TYPE_CAMERA) 
	{
		m_viewBigVideoRender.Clear();
		UpadateVideoRenderInfo();
	}

	ShowOperationTips(m_operationTipsCloseCameraing, 500);
	m_isEnableCamera = false;
	int retCode = m_sdkWrapper.CloseCamera();
	if(retCode != AV_OK)ClearOperationTips();

	if(retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenCamera.EnableWindow(TRUE);
		m_btnCloseCamera.EnableWindow(FALSE);
#endif
		m_isEnableCamera = true;
	}

	ShowRetCode("CloseCamera", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnStartAccompany()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHK_ENABLE_SYSTEM_ACCOMPANY);
	int state = pBtn->GetCheck();

	int retCode = AV_OK;
	bool isEnableSys = false;
	if (state == BST_CHECKED)
	{
		isEnableSys = true;
	}
	else if (state == BST_UNCHECKED)
	{
		isEnableSys = false;
	}

	if (isEnableSys)
	{
		retCode = m_sdkWrapper.StartAccompany("", "", AVAccompanyDevice::AV_ACCOMPANY_SOURCE_TYPE_SYSTEM);
		ShowRetCode("StartAccompany", retCode);
	} 
	else
	{
		CString strPlayerPath = L"";
		CString strMediaPath = L"";

		m_editPlayerPath.GetWindowTextW(strPlayerPath);
		m_editMediaPath.GetWindowTextW(strMediaPath);

		//m_sdkWrapper.StartAccompany("C:\\Program Files (x86)\\Tencent\\QQMusic\\QQMusic1152.19.34.25\\QQMusic.exe", "D:\\download\\时间去哪儿.mp3", AVAccompanyDevice::ACCOMPANY_SOURCE_TYPE_PROCESS);
		if (strPlayerPath.GetLength() > 0)
		{
			retCode = m_sdkWrapper.StartAccompany(StrWToStrA(strPlayerPath).c_str(), StrWToStrA(strMediaPath).c_str(), AVAccompanyDevice::ACCOMPANY_SOURCE_TYPE_PROCESS);
			ShowRetCode("StartAccompany", retCode);
		}
		else
		{
			//ERROR
			retCode = AV_ERR_FAILED;
			//TODO add by darrenhe
			ShowRetCode("StartAccompanyError", AV_ERR_FAILED);
		}
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnStopAccompany()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
	int retCode = m_sdkWrapper.StopAccompany();
	ShowRetCode("StopAccompany", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnOpenMic()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenMic.EnableWindow(FALSE);
#endif
	m_isEnableMic = true;
	int retCode = m_sdkWrapper.OpenMic();
	if(retCode == AV_OK)
	{
		uint32 micVolume = m_sdkWrapper.GetMicVolume();
		CString strMicVolume = _T("");
		strMicVolume.Format(_T("%d"), micVolume);
		m_sliderAudioMicVolume.SetPos(micVolume);
		m_staticMicCurVolume.SetWindowTextW(strMicVolume);
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenMic.EnableWindow(TRUE);
#endif
		m_isEnableMic = false;
	}

	ShowRetCode("OpenMic", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnCloseMic()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnCloseMic.EnableWindow(FALSE);
#endif
	m_isEnableMic = false;
	int retCode = m_sdkWrapper.CloseMic();		
	if(retCode == AV_OK)
	{
		uint32 micVolume = 0;
		CString strMicVolume = _T("");
		strMicVolume.Format(_T("%d"), micVolume);
		m_sliderAudioMicVolume.SetPos(micVolume);
		m_staticMicCurVolume.SetWindowTextW(strMicVolume);
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnCloseMic.EnableWindow(TRUE);
#endif
		m_isEnableMic = true;
	}

	ShowRetCode("CloseMic", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenPlayer()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenPlayer.EnableWindow(FALSE);
#endif
	m_isEnablePlayer = true;
	int retCode = m_sdkWrapper.OpenPlayer();			
	if(retCode == AV_OK)
	{
		uint32 playerVolume = m_sdkWrapper.GetPlayerVolume();
		CString strPlayerVolume = _T("");
		strPlayerVolume.Format(_T("%d"), playerVolume);
		m_sliderAudioPlayerVolume.SetPos(playerVolume);
		m_staticPlayerCurVolume.SetWindowTextW(strPlayerVolume);	
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenPlayer.EnableWindow(TRUE);
#endif
		m_isEnablePlayer = false;
	}

	ShowRetCode("OpenPlayer", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnClosePlayer()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnClosePlayer.EnableWindow(FALSE);
#endif
	m_isEnablePlayer = false; 
	int retCode = m_sdkWrapper.ClosePlayer();		
	if(retCode == AV_OK)
	{
		uint32 playerVolume = 0;
		CString strPlayerVolume = _T("");
		strPlayerVolume.Format(_T("%d"), playerVolume);
		m_sliderAudioPlayerVolume.SetPos(playerVolume);
		m_staticPlayerCurVolume.SetWindowTextW(strPlayerVolume);	
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnClosePlayer.EnableWindow(TRUE);
#endif
		m_isEnablePlayer = true;
	}

	ShowRetCode("ClosePlayer", retCode);
}


void DialogQAVSDKDemo::OnBnClickedChkEnableAec()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHK_ENABLE_AEC);
	int state = pBtn->GetCheck();

	bool isEnable = false;
	if (state == BST_CHECKED)
	{
		isEnable = true;
	}
	else if (state == BST_UNCHECKED)
	{
		isEnable = false;
	}

	m_sdkWrapper.EnableAEC(isEnable);
}


void DialogQAVSDKDemo::OnBnClickedChkEnableNs()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHK_ENABLE_NS);
	int state = pBtn->GetCheck();

	bool isEnable = false;
	if (state == BST_CHECKED)
	{
		isEnable = true;
	}
	else if (state == BST_UNCHECKED)
	{
		isEnable = false;
	}

	m_sdkWrapper.EnableNS(isEnable);
}


void DialogQAVSDKDemo::OnBnClickedChkEnableBoost()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHK_ENABLE_BOOST);
	int state = pBtn->GetCheck();

	bool isEnable = false;
	if (state == BST_CHECKED)
	{
		isEnable = true;
	}
	else if (state == BST_UNCHECKED)
	{
		isEnable = false;
	}

	m_sdkWrapper.EnableBoost(isEnable);
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenAVQualityTips()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}	

	OpenAVQualityTips();
}


void DialogQAVSDKDemo::OnBnClickedBtnCloseAVQualityTips()
{
	CloseAVQualityTips();
}

void DialogQAVSDKDemo::OpenAVQualityTips()
{
	if(m_pAVQualityTipsDlg == NULL)
	{
		SetTimer(TIMER_ENABLE_QUALITY_TIPS, 2000, 0);
		m_pAVQualityTipsDlg = new DialogAVQualityTips();
		m_pAVQualityTipsDlg->Create(IDD_DIALOG_AV_QUALITY_TIPS, this);
		m_pAVQualityTipsDlg->ShowWindow(SW_SHOW | SW_NORMAL);
	}
	else
	{
		m_pAVQualityTipsDlg->ShowWindow(SW_SHOW | SW_NORMAL);
	}

	std::string qualityTips = m_sdkWrapper.GetQualityTips();
	m_pAVQualityTipsDlg->UpdateTips(CString(qualityTips.c_str()));
}

void DialogQAVSDKDemo::CloseAVQualityTips()
{
	KillTimer(TIMER_ENABLE_QUALITY_TIPS);
	if (m_pAVQualityTipsDlg)
	{		
		delete m_pAVQualityTipsDlg;
		m_pAVQualityTipsDlg = NULL;
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnEnableTestEnv()
{
	if(m_isLogin)
	{
		ShowMessageBox(_T("已经登录，先退出登录再进行该操作！"));
		return;
	}

	//切换后台环境
	m_isTestEnv = !m_isTestEnv;
	//TIMManager::get().set_env(m_isTestEnv ? 1 : 0);
	if(!m_isTestEnv)
	{
		m_btnEnableTestEnv.SetWindowTextW(_T("进入测试环境"));
	}
	else
	{
		m_btnEnableTestEnv.SetWindowTextW(_T("进入正式环境"));
	}	

	//先初始化通信SDK
	//TIMManager::get().Init();
	m_btnEnableTestEnv.EnableWindow(FALSE);
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenUserManual()
{
	ShowMessageBox(_T("暂不支持。"));
	return;

	CString userManualFilePath;	
	userManualFilePath.Format(_T("%susermanual.mht"),GetExePath());
	int ret_code = (int)ShellExecute(NULL, NULL, userManualFilePath, NULL, NULL, SW_SHOWMAXIMIZED);
	if(ret_code <= 32)//打开失败
	{
		ShowMessageBox(_T("打开手册失败，请手动用网页浏览器打开DEMO可执行性文件QAVSDKDemo.exe所在目录下的DEMO使用手册usermanual.mht。"));
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnSnapShot()
{
  //To Do:
  SYSTEMTIME time = {0};
  ::GetLocalTime(&time);

  char strTime[256] = {0};
  sprintf(strTime, "%d-%d-%d_%d-%d-%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

  std::string localViewFileName = "local_view_" + std::string(strTime) + ".bmp";
  std::string remoteViewFileName = "remote_view_" + std::string(strTime) + ".bmp";

  m_viewLocalVideoRender.SnapShot(localViewFileName.c_str());
  m_viewBigVideoRender.SnapShot(remoteViewFileName.c_str());
}


void DialogQAVSDKDemo::OnEnChangeEditRoomId()
{
	CString roomId;
	m_editRoomId.GetWindowTextW(roomId);
	ConfigInfoMgr::GetInst()->SetRoomId(roomId);
}

void DialogQAVSDKDemo::OnCbnSelchangeComboSceneTypeList()
{
	CString sceneType;
	m_comboSceneTypeList.GetWindowTextW(sceneType);
	AppInfolist appInfoList = ConfigInfoMgr::GetInst()->GetAppInfoList();	
	if (appInfoList.size() > 0)
	{
		for (int i = 0; i < appInfoList.size(); i++)
		{
			if(appInfoList[i].sceneType == sceneType)
			{
				m_curSceneType = sceneType;
				m_comboSceneTypeList.SetWindowTextW(appInfoList[i].sceneType);
				m_editAccountType.SetWindowTextW(appInfoList[i].accountType);
				m_editAppIdAt3rd.SetWindowTextW(appInfoList[i].appIdAt3rd);
				m_editSdkAppId.SetWindowTextW(appInfoList[i].sdkAppId);
			}
		}		
	}
}


void DialogQAVSDKDemo::PrepareEncParam()
{

}

void DialogQAVSDKDemo::ShowOperationTips(CString tips, int timeLen)
{
	KillTimer(TIMER_SHOW_OPERRATION_TIPS);
	m_staticOperationTips.SetWindowTextW(tips);
	SetTimer(TIMER_SHOW_OPERRATION_TIPS, timeLen, 0);
}


void DialogQAVSDKDemo::ClearOperationTips()
{
	m_staticOperationTips.SetWindowTextW(_T(""));
}

void DialogQAVSDKDemo::ChangeAuthComplete(int retCode)
{
  m_btnProtoAuth.EnableWindow(TRUE);
  m_btnProtoAuth.SetWindowText(L"修改权限");

  if(retCode != AV_OK)
    ShowRetCode("修改权限失败", retCode);
}

void DialogQAVSDKDemo::UpdateEndpointList()
{
	AVEndpoint** endpointList[1];
	int count = m_sdkWrapper.GetEndpointList(endpointList);
	if (count == 0) return;

	m_listEndpointList.ResetContent();
	for (int i = 0 ; i < count; i++)
	{
		std::string identifier = endpointList[0][i]->GetId();

		if(identifier.size() == 0)
		{
			//ShowRetCode("UserUpdate identifier.size() == 0", AV_ERR_FAILED);
			continue;
		}

		int j = 0;
		for(j = 0; j < i; j++)
		{
			std::string identifierTmp = endpointList[0][j]->GetId();
			if(identifier == identifierTmp)//repeat
			{
				break;
			}
		}

		if(j < i)//repeat
		{
			//ShowRetCode("UserUpdate identifier repeat", AV_ERR_FAILED);
			continue;
		}

		AVEndpoint *endpoint = m_sdkWrapper.GetEndpointById(identifier);
		if(endpoint)
		{
			if(endpoint->HasAudio() || endpoint->HasCameraVideo() || endpoint->HasScreenVideo())
			{
				//加上状态标志
				CString itemText = _T("");
				itemText.Format(_T("%s(%s%s,%s%s%s)"), CString(identifier.c_str()), 
				identifier == m_config.identifier ? _T("自己,") : _T(""), 
				endpoint->HasAudio() ? _T("正在说话") : _T("没说话"),
				endpoint->HasCameraVideo() || endpoint->HasScreenVideo() ? _T("正在视频") : _T("没视频"),
				endpoint->HasCameraVideo() ? _T("-摄像头") : _T(""),
				endpoint->HasScreenVideo() ? _T("-屏幕分享") : _T(""));
				m_listEndpointList.AddString(itemText);
			}			
		}
		else
		{
			//ShowRetCode("UserUpdate endpoint == NULL", AV_ERR_FAILED);
		}
	}
}

void DialogQAVSDKDemo::UpadateVideoRenderInfo()
{
	std::vector<std::string> identifierList = m_sdkWrapper.GetRequestViewIdentifierList();
	std::vector<View> viewList = m_sdkWrapper.GetRequestViewParamList();

	bool bNeedClearBig = true,  bNeedClear1 = true, bNeedClear2 = true,  bNeedClear3 = true,  bNeedClear4 = true, bNeedClearScreen = true;
	for (int i = 0; i < identifierList.size(); i++)
	{
		AVEndpoint *endpoint = m_sdkWrapper.GetEndpointById(identifierList[i]);
		bool hasVideo = endpoint != NULL && ((viewList[i].video_src_type == VIDEO_SRC_TYPE_CAMERA && endpoint->HasCameraVideo()) 
			|| (viewList[i].video_src_type == VIDEO_SRC_TYPE_SCREEN && endpoint->HasScreenVideo()));

		if(m_viewBigVideoRender.m_identifier == identifierList[i] 
			&& m_viewBigVideoRender.m_videoSrcType == viewList[i].video_src_type && hasVideo)bNeedClearBig = false;		
		else if(m_viewScreenShareRender.m_identifier == identifierList[i] 
			&& m_viewScreenShareRender.m_videoSrcType == viewList[i].video_src_type && hasVideo)bNeedClearScreen = false;
	}

	if(m_viewBigVideoRender.m_identifier == m_config.identifier)bNeedClearBig = false;//如果显示着自己的画面，也不应该清除。
	if(m_viewScreenShareRender.m_identifier == m_config.identifier)bNeedClearScreen = false;//如果显示着自己的画面，也不应该清除。

	if(bNeedClearBig)m_viewBigVideoRender.Clear();
	if(bNeedClearScreen)m_viewScreenShareRender.Clear();

	for (int i = 0; i < identifierList.size(); i++)
	{
		if(viewList[i].video_src_type == VIDEO_SRC_TYPE_SCREEN)
		{
			if(m_viewScreenShareRender.m_identifier.empty())
			{
				m_viewScreenShareRender.m_identifier = identifierList[i];
				m_viewScreenShareRender.m_videoSrcType = viewList[i].video_src_type;
				continue;
			}
		}		
	}

	if(m_viewBigVideoRender.m_identifier == "")//如果大画面窗口没人用，就自动给某路用。
	{
		if(m_viewScreenShareRender.m_identifier != "")
		{
			m_viewBigVideoRender.m_identifier = m_viewScreenShareRender.m_identifier;
			m_viewBigVideoRender.m_videoSrcType = m_viewScreenShareRender.m_videoSrcType;
		}
		else if(m_viewLocalVideoRender.m_identifier != "")
		{
			m_viewBigVideoRender.m_identifier = m_viewLocalVideoRender.m_identifier;
			m_viewBigVideoRender.m_videoSrcType = m_viewLocalVideoRender.m_videoSrcType;
		}
	}
}

void DialogQAVSDKDemo::UpdateConfigInfo()
{
	ConfigInfoMgr::GetInst()->LoadConfigInfo();

	AppInfolist appInfoList = ConfigInfoMgr::GetInst()->GetAppInfoList();	
	if (appInfoList.size() > 0)
	{
		m_comboSceneTypeList.ResetContent();
		int i = 0;
		for (int i = 0; i < appInfoList.size(); i++)
		{
			m_comboSceneTypeList.InsertString(i, appInfoList[i].sceneType);
			if(appInfoList[i].sceneType == m_curSceneType)
			{
				m_comboSceneTypeList.SetWindowTextW(appInfoList[i].sceneType);
				m_comboSceneTypeList.SetCurSel(i);
				m_editAccountType.SetWindowTextW(appInfoList[i].accountType);
				m_editAppIdAt3rd.SetWindowTextW(appInfoList[i].appIdAt3rd);
				m_editSdkAppId.SetWindowTextW(appInfoList[i].sdkAppId);
			}
			else 
			{
				m_comboSceneTypeList.SetWindowTextW(appInfoList[0].sceneType);
				m_comboSceneTypeList.SetCurSel(0);
				m_editAccountType.SetWindowTextW(appInfoList[0].accountType);
				m_editAppIdAt3rd.SetWindowTextW(appInfoList[0].appIdAt3rd);
				m_editSdkAppId.SetWindowTextW(appInfoList[0].sdkAppId);
			}
		}	
	}
	else 
	{
		m_comboSceneTypeList.ResetContent();
		m_comboSceneTypeList.SetWindowTextW(_T(""));
		m_editAccountType.SetWindowTextW(_T(""));
		m_editAppIdAt3rd.SetWindowTextW(_T(""));
		m_editSdkAppId.SetWindowTextW(_T(""));	
	}

	CString roomId = ConfigInfoMgr::GetInst()->GetRoomId();
	m_editRoomId.SetWindowTextW(roomId);

	AccountInfolist accountInfoList = ConfigInfoMgr::GetInst()->GetAccountInfoList();
	if (accountInfoList.size() > 0)
	{
		m_comboAccountList.ResetContent();
		m_comboAccountList.SetWindowTextW(accountInfoList[0].identifier);

		for (int i = 0; i < accountInfoList.size(); i++)
		{
			m_comboAccountList.InsertString(i, accountInfoList[i].identifier);
		}
		m_comboAccountList.SetCurSel(0);
	}

	//从这以后都是自己加的
	ChannelInfolist channelInfoList = ConfigInfoMgr::GetInst()->GetChannelInfoList();
	if (channelInfoList.size() > 0)
	{
		m_comboChannelList.ResetContent();
		m_comboChannelList.SetWindowTextW(channelInfoList[0].channelName);

		for (int i = 0; i < channelInfoList.size(); i++)
		{
			m_comboChannelList.InsertString(i, channelInfoList[i].channelName);
		}
		m_comboChannelList.SetCurSel(0);
	}
}



void DialogQAVSDKDemo::ShowRetCode(std::string tipsStr, int retCode)
{
  if (retCode != AV_OK)
  {
    CString errInfo = _T("");
    switch (retCode)
    {
    case AV_ERR_FAILED: 
      errInfo = _T("AV_ERR_FAILED");
      break;
    
    case AV_ERR_INVALID_ARGUMENT: 
      errInfo = _T("AV_ERR_INVALID_ARGUMENT");
      break;
    
    case AV_ERR_TIMEOUT: 
      errInfo = _T("AV_ERR_TIMEOUT");
      break;
    case AV_ERR_NOT_IMPLEMENTED: 
      errInfo = _T("AV_ERR_NOT_IMPLEMENTED");
      break;
    
   
    case AV_ERR_NOT_IN_MAIN_THREAD: 
      errInfo = _T("AV_ERR_NOT_IN_MAIN_THREAD");
      break;

      //CONTEXT相关(-1401 to -1500)      
    case AV_ERR_CONTEXT_NOT_EXIST: 
      errInfo = _T("AV_ERR_CONTEXT_NOT_EXIST");
      break;

      //房间相关(-1501 to -1600)     
    
    case AV_ERR_ROOM_NOT_EXIST: 
      errInfo = _T("AV_ERR_ROOM_NOT_EXIST");
      break;
    case AV_ERR_ROOM_NOT_EXITED: 
      errInfo = _T("AV_ERR_ROOM_NOT_EXITED");
      break;	

      //设备相关(-1601 to -1700)   
   
    case AV_ERR_DEVICE_NOT_EXIST: 
      errInfo = _T("AV_ERR_DEVICE_NOT_EXIST");
      break;
    case AV_ERR_ENDPOINT_HAS_NOT_VIDEO: 
      errInfo = _T("AV_ERR_ENDPOINT_HAS_NOT_VIDEO");
      break;	
    case AV_ERR_REPEATED_OPERATION: 
      errInfo = _T("AV_ERR_REPEATED_OPERATION");
      break;   
    case AV_ERR_HAS_IN_THE_STATE: 
      errInfo = _T("AV_ERR_HAS_IN_THE_STATE");
      break;		

      //成员相关(-1701 to -1800)
   
    case AV_ERR_ENDPOINT_NOT_EXIST: 
      errInfo = _T("AV_ERR_ENDPOINT_NOT_EXIST");
      break;
  
    default: 
      errInfo = _T("其他错误");
      break;
    }

    CString errCodeStr;
    errCodeStr.Format(_T("时出错！错误码：%d, 错误提示信息: %s."), retCode, errInfo);
    errCodeStr = CString(tipsStr.c_str()) + errCodeStr;
    ShowMessageBox(errCodeStr);
  }
}

void DialogQAVSDKDemo::OnCbnSelchangeComboCameraList()
{
	int selIdx = m_comboCameraList.GetCurSel();
	if(m_sdkWrapper.GetSelectedCameraId() == m_cameraList[selIdx].first)return;
	m_sdkWrapper.SetSelectedCameraId(m_cameraList[selIdx].first);
	OnBnClickedBtnOpenCamera();
}


void DialogQAVSDKDemo::OnCbnSelchangeComboMicList()
{
	int selIdx = m_comboMicList.GetCurSel();
	m_sdkWrapper.SetSelectedMicId(m_micList[selIdx].first);
	OnBnClickedBtnOpenMic();
}


void DialogQAVSDKDemo::OnCbnSelchangeComboPlayerList()
{
	int selIdx = m_comboPlayerList.GetCurSel();
	if(m_sdkWrapper.GetSelectedPlayerId() == m_playerList[selIdx].first)return;
	m_sdkWrapper.SetSelectedPlayerId(m_playerList[selIdx].first);
	OnBnClickedBtnOpenPlayer();
}


void DialogQAVSDKDemo::OnBnClickedBtnProaut()
{
  typedef struct  {
    char c;
    char lv;
    char hv;
  } privilege_map_entry;

  static const privilege_map_entry dataPrivilegeMap[16] = {
    {'0',0x0,0x0}, {'1',0x01,0x10},{'2',0x02,0x20},{'3',0x03,0x30},
    {'4',0x04,0x40},{'5',0x05,0x50},{'6',0x06,0x60},{'7',0x07,0x70},
    {'8',0x08,0x80},{'9',0x09,0x90},{'A',0x0A,0xA0},{'B',0x0B,0xB0},
    {'C',0x0C,0xC0},{'D',0x0D,0xD0},{'E',0x0E,0xE0},{'F',0x0F,0xF0},
  };

   CString cstrProtoAuthBuffer; 
   std::string strPrivilegeMap;

	  m_editProtoAuthBuffer.GetWindowText(cstrProtoAuthBuffer);

	  std::string strProtoAuthBuffer = StrWToStrA(cstrProtoAuthBuffer);


	  	uint64 auth_bit = 0xff;//明文默认全开
	CString cstrProtoAuthBit; 

	m_editProtoAuthBit.GetWindowText(cstrProtoAuthBit);

	std::string strProtoAuthBit = StrWToStrA(cstrProtoAuthBit);

	  if (!strProtoAuthBuffer.size() && !strProtoAuthBit.size())
	  {
		  ::MessageBox(0, L"请填写权限位或加密串", L"动态修改权限", 0);
	  }
	  else
	  {
 
	if (strProtoAuthBuffer.size())
	{
	
	  if(strProtoAuthBuffer.size() % 2 != 0) {
		::MessageBox(0, L"权限位加密串长度有误(必须为2的倍数)", L"动态修改权限", 0);
		return;
	  }

	  for(int n=0; n < strProtoAuthBuffer.size(); n+=2) {
		char v = 0x00;

		bool bValid = false;
		for(int i=0; i<16; i++) {
		  if(strProtoAuthBuffer[n] == dataPrivilegeMap[i].c)
		  {
			v |= dataPrivilegeMap[i].hv;
			bValid = true;
			break;
		  }
		}
		if(!bValid) {
		  ::MessageBox(0, L"权限位加密串有非法字符(16进制)", L"动态修改权限", 0);
		  return;
		}

		bValid = false;
		for(int i=0; i<16; i++) {
		  if(strProtoAuthBuffer[n+1] == dataPrivilegeMap[i].c)
		  {
			v |= dataPrivilegeMap[i].lv;
			bValid = true;
			break;
		  }
		}
		if(!bValid) {
		  ::MessageBox(0, L"权限位加密串有非法字符(16进制)", L"动态修改权限", 0);
		  return;
		}

		strPrivilegeMap.append(1, v);
	  }
	}


    if (strProtoAuthBit.size())
	{

		  stringstream strValue;

	
		  strValue <<hex<< strProtoAuthBit;

		  strValue >> auth_bit;

	}


  int retCode = m_sdkWrapper.ChangeAuthority(auth_bit,strPrivilegeMap);
  if(AV_OK == retCode)
  {
    m_btnProtoAuth.SetWindowText(L"正在修改权限");
    m_btnProtoAuth.EnableWindow(FALSE);
  }
  else
  {
    ShowRetCode("修改权限失败", retCode);
  }

  }
}

void DialogQAVSDKDemo::PrepareYUVData()
{
    if (!m_pYUVData)
    {
        LoadBmpData("CameraTabBtnPushed.bmp", &m_pYUVData, m_nYUVWidth, m_nYUVHeight);
    }

    if (!m_pYUVCapData)
    {
        LoadBmpData("TestExternalCapture.bmp", &m_pYUVCapData, m_nYUVCapWidth, m_nYUVCapHeight);
    }    
}

void DialogQAVSDKDemo::UpdateAudioDataState()
{
	AVAudioCtrl::AudioDataSourceType type = (AVAudioCtrl::AudioDataSourceType)m_comboAudioDataType.GetCurSel();
	AudioFrameDesc audio_desc;
	m_sdkWrapper.GetAudioDataFormat(type, audio_desc);
	bool isEnable = m_sdkWrapper.IsEnableAudioData(type);

	int i= 0;
	for (i = 0; i < sizeof(g_AudioDataSampleRates) / sizeof(g_AudioDataSampleRates[0]); i++)
	{
		if(audio_desc.sample_rate == g_AudioDataSampleRates[i].value)
		{
			m_comboAudioDataSampleRate.SetCurSel(i);
			break;
		}
	}

	for (i = 0; i < sizeof(g_AudioDataChannelNumStates) / sizeof(g_AudioDataChannelNumStates[0]); i++)
	{
		if(audio_desc.channel_num == g_AudioDataChannelNumStates[i].value)
		{
			m_comboAudioDataChannelNum.SetCurSel(i);
			break;
		}
	}
	/*
	for (i = 0; i < sizeof(g_AudioDataEnableStates) / sizeof(g_AudioDataEnableStates[0]); i++)
	{
		if(isEnable == g_AudioDataEnableStates[i].value)
		{
			m_comboAudioDataEnable.SetCurSel(i);
			break;
		}
	}
	*/

	float volume = 0; m_sdkWrapper.GetAudioDataVolume(type, &volume);
	m_sliderAudioDataVolume.SetPos((int)(volume * MAX_AUDIO_DEVICE_VOLUME));
}

void DialogQAVSDKDemo::OnVideoPreTreatment(VideoFrame* pFrame)
{
#ifdef TEST_FOR_INTERNAL
    if (m_bEnablePreTreat
        && pFrame 
        && pFrame->data 
        && pFrame->data_size 
        && pFrame->desc.color_format == COLOR_FORMAT_I420
        && m_pYUVData)
    {
        AddImg_I420(pFrame->data, pFrame->desc.width, pFrame->desc.height, m_pYUVData, m_nYUVWidth, m_nYUVHeight);
    }
#endif
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenExteralCapture()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenExternalCapture.EnableWindow(FALSE);
	m_btnCloseExternalCapture.EnableWindow(TRUE);
#endif

	int retCode = m_sdkWrapper.OpenExternalCapture();
	if (retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenExternalCapture.EnableWindow(TRUE);
		m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif
	}

	ShowRetCode("OpenExternalCapture", retCode);
	SetTimer(TIMER_EXTERNAL_CAPTURE, 66, 0);
}


void DialogQAVSDKDemo::OnBnClickedBtnCloseExteralCapture()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenExternalCapture.EnableWindow(TRUE);
	m_btnCloseExternalCapture.EnableWindow(FALSE);
#endif

	int retCode = m_sdkWrapper.CloseExternalCapture();
	if (retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenExternalCapture.EnableWindow(FALSE);
		m_btnCloseExternalCapture.EnableWindow(TRUE);
#endif
	}

	ShowRetCode("CloseExternalCapture", retCode);
	KillTimer(TIMER_EXTERNAL_CAPTURE);
	if(m_pYuvInputFile)
	{
		fclose(m_pYuvInputFile);
		m_pYuvInputFile = NULL;
	}
}


void DialogQAVSDKDemo::OnBnClickedEnablepretreat()
{
  // TODO: Add your control notification handler code here
  m_bEnablePreTreat = TRUE;
}


void DialogQAVSDKDemo::OnBnClickedDisablepretreat()
{
  // TODO: Add your control notification handler code here
  m_bEnablePreTreat = FALSE;
}


LONG DialogQAVSDKDemo::OnAudioDeviceDetect(WPARAM wParam, LPARAM lParam)
{
	bool isSelect = (bool)wParam;
	DetectedDeviceInfo*pInfo = (DetectedDeviceInfo*)lParam;
	CString strDevice = CString(pInfo->strGuid.c_str());

	if (pInfo->flow == Detect_Mic)
	{
		int selCur = -1;
		string strCurDevice;

		if (m_micList.size() > 0){
			selCur = m_comboMicList.GetCurSel();
			if (selCur >= 0 && m_micList.size() > selCur){
				strCurDevice = m_micList[selCur].second;
			}
		}

		m_micList.clear();
		m_sdkWrapper.GetMicList(m_micList);
		m_comboMicList.ResetContent();
		
		if(m_micList.size() > 0)
		{
			int index = -1;
			int curIndex = -1;
			for (int i = 0; i < m_micList.size(); i++)
			{
				if (m_micList[i].first == pInfo->strGuid)
					index = i;
				if (m_micList[i].second == strCurDevice)
					curIndex = i;

				if(i == 0)m_comboMicList.InsertString(i, _T("(Windows默认设备)") + CString(m_micList[i].second.c_str()));
				else m_comboMicList.InsertString(i, CString(m_micList[i].second.c_str()));      
			}

			//恢复一下选项
			if (!isSelect){
				if (!pInfo->isNewDevice && pInfo->isUsedDevice)
					m_comboMicList.SetCurSel(-1);
				else
					m_comboMicList.SetCurSel(curIndex);
			}
			else{
				if (pInfo->isNewDevice)
					m_comboMicList.SetCurSel(index);
				else
					m_comboMicList.SetCurSel(0);
			}
		}
	}

	else if (pInfo->flow == Detect_Speaker){
		int selCur = -1;
		string strCurDevice;

		if (m_playerList.size() > 0){
			selCur = m_comboPlayerList.GetCurSel();
			strCurDevice = m_playerList[selCur].second;
		}

		m_playerList.clear();
		m_sdkWrapper.GetPlayerList(m_playerList);
		m_comboPlayerList.ResetContent();
		if(m_playerList.size() > 0)
		{	
			int index = -1;
			int curIndex = -1;

			for (int i = 0; i < m_playerList.size(); i++)
			{
				if (pInfo->strGuid == m_playerList[i].first)
					index = i;

				if (m_playerList[i].second == strCurDevice)
					curIndex = i;

				if(i == 0)m_comboPlayerList.InsertString(i, _T("(Windows默认设备)") + CString(m_playerList[i].second.c_str()));
				else m_comboPlayerList.InsertString(i, CString(m_playerList[i].second.c_str()));  
			}

			//恢复一下选项
			if (!isSelect){
				if (!pInfo->isNewDevice && pInfo->isUsedDevice)
					m_comboPlayerList.SetCurSel(-1);
				else
					m_comboPlayerList.SetCurSel(curIndex);
			}
			else{
				if (pInfo->isNewDevice)
					m_comboPlayerList.SetCurSel(index);
				else
					m_comboPlayerList.SetCurSel(0);
			}
		}
	}

	delete pInfo;
	return 0;

}

LONG DialogQAVSDKDemo::OnLButtonDBClick(WPARAM wParam, LPARAM lParam)
{
	int px = GET_X_LPARAM(lParam);
	int py = GET_Y_LPARAM(lParam);

	RECT rectParent = {0};
	GetWindowRect(&rectParent);

	RECT rect = {0};

	m_viewLocalVideoRender.GetWindowRect(&rect);
	rect.top -= rectParent.top;
	rect.bottom -= rectParent.top;
	rect.left -= rectParent.left;
	rect.right -= rectParent.left;

	if( rect.top < py && rect.bottom > py &&
		rect.left < px && rect.right > px)
	{
		m_viewBigVideoRender.m_identifier = m_viewLocalVideoRender.m_identifier;
		m_viewBigVideoRender.m_videoSrcType = m_viewLocalVideoRender.m_videoSrcType;
		return 0;
	}

	m_viewScreenShareRender.GetWindowRect(&rect);
	rect.top -= rectParent.top;
	rect.bottom -= rectParent.top;
	rect.left -= rectParent.left;
	rect.right -= rectParent.left;
	if( rect.top < py && rect.bottom > py &&
		rect.left < px && rect.right > px)
	{
		if(!m_viewScreenShareRender.m_identifier.empty())
		{
			m_viewBigVideoRender.m_identifier = m_viewScreenShareRender.m_identifier;
			m_viewBigVideoRender.m_videoSrcType = m_viewScreenShareRender.m_videoSrcType;
		}
		return 0;
	}

	return 0;
}

void DialogQAVSDKDemo::SaveRemoteYuvFrame(VideoFrame *pFrameData)
{
  if( m_rectYuvOutputWidth != pFrameData->desc.width ||
      m_rectYuvOutputHeight != pFrameData->desc.height )
  {
    if(m_pYuvOutputFile)
    {
      fclose(m_pYuvOutputFile);
      m_pYuvOutputFile = NULL;
    }

    m_rectYuvOutputWidth = pFrameData->desc.width;
    m_rectYuvOutputHeight = pFrameData->desc.height;
  }

  if(!m_pYuvOutputFile)
  {
     SYSTEMTIME time = {0};
    ::GetLocalTime(&time);

    char strTime[256] = {0};
    sprintf(strTime, "%d-%d-%d_%d-%d-%d_%d-%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, m_rectYuvOutputWidth, m_rectYuvOutputHeight);
    std::string remoteViewFileName = "remote_" + std::string(strTime) + ".yuv";

    char szBmpPath[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, szBmpPath, MAX_PATH);
    PathAppendA(szBmpPath, "..\\");
    PathAppendA(szBmpPath, remoteViewFileName.c_str());

    m_pYuvOutputFile = fopen(szBmpPath, "wb+");
  }

  fwrite(pFrameData->data, 1, pFrameData->data_size, m_pYuvOutputFile);
}

void DialogQAVSDKDemo::OnBnClickedBtnStartDeviceTest()
{
	if(!m_isLogin)
	{
		ShowMessageBox(_T("没有登录，不能进行设备测试！"));
		return;
	}

	if(m_isEnterRoom)
	{
		ShowMessageBox(_T("在房间中，不能进行设备测试。请先退出房间再进行测试！"));
		return;
	}

	int retCode = m_sdkWrapper.StartDeviceTest();
	if(retCode != AV_OK)ClearOperationTips();
#ifdef ENABLE_UI_OPERATION_SAFETY
	if(retCode != AV_OK)m_btnStartDeviceTest.EnableWindow(TRUE);
#endif
	ShowRetCode("StartDeviceTest", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnStopDeviceTest()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

	int retCode = m_sdkWrapper.StopDeviceTest();
	if(retCode != AV_OK)ClearOperationTips();
#ifdef ENABLE_UI_OPERATION_SAFETY
	if(retCode != AV_OK)m_btnStopDeviceTest.EnableWindow(TRUE);
#endif
	ShowRetCode("StopDeviceTest", retCode);
}


LONG DialogQAVSDKDemo::OnStartDeviceTest(WPARAM wParam, LPARAM lParam)
{
	Error nError = (Error)wParam;
	if (AV_OK == nError)
	{
		m_isStartDeviceTest = true;
		m_btnStartDeviceTest.EnableWindow(FALSE);
		m_btnStopDeviceTest.EnableWindow(TRUE);
		m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
		m_btnDeviceTestCloseCamera.EnableWindow(TRUE);
		m_btnDeviceTestOpenMic.EnableWindow(TRUE);
		m_btnDeviceTestCloseMic.EnableWindow(TRUE);
		m_btnDeviceTestOpenPlayer.EnableWindow(TRUE);
		m_btnDeviceTestClosePlayer.EnableWindow(TRUE);
		
		m_deviceTestMicList.clear();
		m_deviceTestPlayerList.clear();
		m_deviceTestCameraList.clear();
		m_sdkWrapper.DeviceTestGetMicList(m_deviceTestMicList);
		m_sdkWrapper.DeviceTestGetPlayerList(m_deviceTestPlayerList);
		m_sdkWrapper.DeviceTestGetCameraList(m_deviceTestCameraList);

		m_comboDeviceTestMicList.ResetContent();
		m_comboDeviceTestMicList.SetWindowTextW(_T(""));
		if(m_deviceTestMicList.size() > 0)
		{
			for (int i = 0; i < m_deviceTestMicList.size(); i++)
			{
				m_comboDeviceTestMicList.InsertString(i, CString(m_deviceTestMicList[i].second.c_str()));      
			}
			m_comboDeviceTestMicList.SetCurSel(0);

			m_sdkWrapper.DeviceTestSetSelectedMicId(m_deviceTestMicList[0].first);
			int retCode = m_sdkWrapper.DeviceTestOpenMic();
			ShowRetCode("DeviceTestOpenMic", retCode);
		}

		m_comboDeviceTestPlayerList.ResetContent();
		m_comboDeviceTestPlayerList.SetWindowTextW(_T(""));
		if(m_deviceTestPlayerList.size() > 0)
		{
			for (int i = 0; i < m_deviceTestPlayerList.size(); i++)
			{
				m_comboDeviceTestPlayerList.InsertString(i, CString(m_deviceTestPlayerList[i].second.c_str()));  
			}
			m_comboDeviceTestPlayerList.SetCurSel(0);

			m_sdkWrapper.DeviceTestSetSelectedPlayerId(m_deviceTestPlayerList[0].first);
			int retCode = m_sdkWrapper.DeviceTestOpenPlayer();
			ShowRetCode("DeviceTestOpenPlayer", retCode);		
		}

		m_comboDeviceTestCameraList.ResetContent();
		m_comboDeviceTestCameraList.SetWindowTextW(_T(""));
		if(m_deviceTestCameraList.size() > 0)
		{
			for (int i = 0; i < m_deviceTestCameraList.size(); i++)
			{
				m_comboDeviceTestCameraList.InsertString(i, CString(m_deviceTestCameraList[i].second.c_str()));      
			}
			m_comboDeviceTestCameraList.SetCurSel(0);

			m_sdkWrapper.DeviceTestSetSelectedCameraId(m_deviceTestCameraList[0].first);
			int retCode = m_sdkWrapper.DeviceTestOpenCamera();
			ShowRetCode("DeviceTestOpenCamera", retCode);
		}
	}
	else
	{
		m_isStartDeviceTest = false;
		m_btnStartDeviceTest.EnableWindow(TRUE);
		m_btnStopDeviceTest.EnableWindow(FALSE);
		m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
		m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
		m_btnDeviceTestOpenMic.EnableWindow(FALSE);
		m_btnDeviceTestCloseMic.EnableWindow(FALSE);
		m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
		m_btnDeviceTestClosePlayer.EnableWindow(FALSE);
 
 		ShowRetCode("StartDeviceTest", nError);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnStopDeviceTest(WPARAM wParam, LPARAM lParam)
{
	m_isStartDeviceTest = false;
	m_btnStartDeviceTest.EnableWindow(TRUE);
	m_btnStopDeviceTest.EnableWindow(FALSE);
	m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
	m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
	m_btnDeviceTestOpenMic.EnableWindow(FALSE);
	m_btnDeviceTestCloseMic.EnableWindow(FALSE);
	m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
	m_btnDeviceTestClosePlayer.EnableWindow(FALSE);
 
	m_viewLocalVideoRender.Clear();
	m_deviceTestMicList.clear();
	m_deviceTestPlayerList.clear();
	m_deviceTestCameraList.clear();
	m_comboDeviceTestCameraList.ResetContent();//先清空
	m_comboDeviceTestCameraList.SetWindowTextW(_T(""));
	m_comboDeviceTestMicList.ResetContent();//先清空
	m_comboDeviceTestMicList.SetWindowTextW(_T(""));
	m_comboDeviceTestPlayerList.ResetContent();//先清空
	m_comboDeviceTestPlayerList.SetWindowTextW(_T(""));

	if(m_isExitDemo)m_sdkWrapper.StopContext();
	return 0;
}


LONG DialogQAVSDKDemo::OnDeviceTestMicOperation(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			uint32 micVolume = m_sdkWrapper.DeviceTestGetMicVolume();
			CString strMicVolume = _T("");
			strMicVolume.Format(_T("%d"), micVolume);
			m_sliderDeviceTestAudioMicVolume.SetPos(micVolume);
			m_isDeviceTestEnableMic = true;
			#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenMic.EnableWindow(FALSE);
			m_btnDeviceTestCloseMic.EnableWindow(TRUE);
			#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnableMic = false;
			#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenMic.EnableWindow(TRUE);
			m_btnDeviceTestCloseMic.EnableWindow(FALSE);
			#endif
			ShowRetCode("DeviceTestMicOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{	
			uint32 micVolume = 0;
			CString strMicVolume = _T("");
			strMicVolume.Format(_T("%d"), micVolume);
			m_sliderDeviceTestAudioMicVolume.SetPos(micVolume);
			m_isDeviceTestEnableMic = false;
			#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenMic.EnableWindow(TRUE);
			m_btnDeviceTestCloseMic.EnableWindow(FALSE);
			#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnableMic = true;
			#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenMic.EnableWindow(FALSE);
			m_btnDeviceTestCloseMic.EnableWindow(TRUE);
			#endif
			ShowRetCode("DeviceTestMicOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("DeviceTestMic", retCode);
	return 0;
}

LONG DialogQAVSDKDemo::OnDeviceTestPlayerOperation(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			uint32 playerVolume = m_sdkWrapper.DeviceTestGetPlayerVolume();
			CString strPlayerVolume = _T("");
			strPlayerVolume.Format(_T("%d"), playerVolume);
			m_sliderDeviceTestAudioPlayerVolume.SetPos(playerVolume);
			m_isDeviceTestEnablePlayer = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
			m_btnDeviceTestClosePlayer.EnableWindow(TRUE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnablePlayer = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenPlayer.EnableWindow(TRUE);
			m_btnDeviceTestClosePlayer.EnableWindow(FALSE);
#endif
			ShowRetCode("DeviceTestPlayerOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{	
			uint32 playerVolume = 0;
			CString strPlayerVolume = _T("");
			strPlayerVolume.Format(_T("%d"), playerVolume);
			m_sliderDeviceTestAudioPlayerVolume.SetPos(playerVolume);
			m_isDeviceTestEnablePlayer = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenPlayer.EnableWindow(TRUE);
			m_btnDeviceTestClosePlayer.EnableWindow(FALSE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnablePlayer = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
			m_btnDeviceTestClosePlayer.EnableWindow(TRUE);
#endif
			ShowRetCode("DeviceTestPlayerOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("DeviceTestPlayer", retCode);
	return 0;
}

LONG DialogQAVSDKDemo::OnDeviceTestCameraOperation(WPARAM wParam, LPARAM lParam)
{
	Error retCode = (Error)wParam;
	AVDevice::DeviceOperation oper = (AVDevice::DeviceOperation)lParam;
	if(oper == AVDevice::DEVICE_OPERATION_OPEN)
	{
		if (AV_OK == retCode)
		{	
			m_isDeviceTestEnableCamera = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
			m_btnDeviceTestCloseCamera.EnableWindow(TRUE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnableCamera = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
			m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
#endif
			ShowRetCode("DeviceTestCameraOperation. ", retCode);
		}
	}
	else if(oper == AVDevice::DEVICE_OPERATION_CLOSE)
	{
		if (AV_OK == retCode)
		{
			m_isDeviceTestEnableCamera = false;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
			m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
#endif
		}
		else if (AV_ERR_FAILED == retCode)
		{
			m_isDeviceTestEnableCamera = true;
#ifdef ENABLE_UI_OPERATION_SAFETY
			m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
			m_btnDeviceTestCloseCamera.EnableWindow(TRUE);
#endif
			ShowRetCode("DeviceTestCameraOperation. ", retCode);
		}
	}

	if(retCode != AV_OK)ShowRetCode("DeviceTestCamera", retCode);
	return 0;
}

LONG DialogQAVSDKDemo::OnPushVideoStart(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		TIMStreamRsp* rsp = (TIMStreamRsp*)(lParam);
		CString info;
		if (rsp->urls.size() > 0)
		{
			CString w = StrAToStrW(rsp->urls.begin()->url.c_str());  //直播地址在这里
			info = _T("开始推流！直播地址：");
			info += w;
		}
		else
		{
			info.Format(_T("推流申请成功！返回信息不足"));
		}
		m_isVideoPushing = TRUE;
		m_uPushVideoChID = rsp->channel_id;
		UpdateVideoPushBtnsState();
		ShowMessageBox(info);
		delete rsp; //回收资源
	}
	else
	{
		int errCode = lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("推流申请出错！错误码：%d."), errCode);
		ShowMessageBox(errCodeStr);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnPushVideoStop(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		CString info;
		info.Format(_T("推流已停止"));
		m_isVideoPushing = FALSE;
		m_uPushVideoChID = 0;
		UpdateVideoPushBtnsState();
		ShowMessageBox(info);
	}
	else
	{
		int errCode = lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("停止推流出错！错误码：%d."), errCode);
		ShowMessageBox(errCodeStr);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnRecordVideoStart(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		CString info;
		info.Format(_T("录制已开始"));
		m_isVideoRecording = TRUE;
		UpdateVideoPushBtnsState();
		ShowMessageBox(info);
	}
	else
	{
		int errCode = lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("开始录制请求出错！错误码：%d."), errCode);
		ShowMessageBox(errCodeStr);
	}
	return 0;
}

LONG DialogQAVSDKDemo::OnRecordVideoStop(WPARAM wParam, LPARAM lParam)
{
	int retCode = wParam;
	if(retCode == AV_OK)
	{
		std::string* file_id = (std::string*)(lParam);
		CString wfile_id = StrAToStrW(file_id->c_str());
		CString info;
		info.Format(_T("录制已停止 fileid:"));
		info += wfile_id;
		m_isVideoRecording = FALSE;
		UpdateVideoPushBtnsState();
		ShowMessageBox(info);
		delete file_id;
	}
	else
	{
		int errCode = lParam;
		CString errCodeStr;
		errCodeStr.Format(_T("停止录制请求出错！错误码：%d."), errCode);
		ShowMessageBox(errCodeStr);
	}
	return 0;
}
void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenCamera()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestOpenCamera.EnableWindow(FALSE);
	m_btnDeviceTestCloseCamera.EnableWindow(TRUE);
#endif
	ShowOperationTips(m_operationTipsOpenCameraing, 1000);
	m_isDeviceTestEnableCamera = true;
	int retCode = m_sdkWrapper.DeviceTestOpenCamera();
	
	if(retCode != AV_OK)ClearOperationTips();

	if(retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
		m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
#endif
		m_isDeviceTestEnableCamera = false;
	}

	ShowRetCode("DeviceTestOpenCamera", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestCloseCamera()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
	m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
#endif
	m_viewLocalVideoRender.Clear();
	ShowOperationTips(m_operationTipsCloseCameraing, 500);
	m_isDeviceTestEnableCamera = false;
	int retCode = m_sdkWrapper.DeviceTestCloseCamera();
	if(retCode != AV_OK)ClearOperationTips();

	if(retCode != AV_OK)
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestOpenCamera.EnableWindow(TRUE);
		m_btnDeviceTestCloseCamera.EnableWindow(FALSE);
#endif
		m_isDeviceTestEnableCamera = true;
	}

	ShowRetCode("DeviceTestCloseCamera", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenMic()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestOpenMic.EnableWindow(FALSE);
#endif
	m_isDeviceTestEnableMic = true;
	int retCode = m_sdkWrapper.DeviceTestOpenMic();
	if(retCode == AV_OK)
	{
		
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestOpenMic.EnableWindow(TRUE);
#endif
		m_isDeviceTestEnableMic = false;
	}

	ShowRetCode("DeviceTestOpenMic", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestCloseMic()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestCloseMic.EnableWindow(FALSE);
#endif
	m_isDeviceTestEnableMic = false;
	int retCode = m_sdkWrapper.DeviceTestCloseMic();		
	if(retCode == AV_OK)
	{
		uint32 micVolume = 0;
		CString strMicVolume = _T("");
		strMicVolume.Format(_T("%d"), micVolume);
		m_sliderDeviceTestAudioMicVolume.SetPos(micVolume);
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestCloseMic.EnableWindow(TRUE);
#endif
		m_isDeviceTestEnableMic = true;
	}

	ShowRetCode("DeviceTestCloseMic", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestOpenPlayer()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestOpenPlayer.EnableWindow(FALSE);
#endif
	m_isDeviceTestEnablePlayer = true;
	int retCode = m_sdkWrapper.DeviceTestOpenPlayer();			
	if(retCode == AV_OK)
	{
		
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestOpenPlayer.EnableWindow(TRUE);
#endif
		m_isDeviceTestEnablePlayer = false;
	}

	ShowRetCode("DeviceTestOpenPlayer", retCode);
}

void DialogQAVSDKDemo::OnBnClickedBtnDeviceTestClosePlayer()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnDeviceTestClosePlayer.EnableWindow(FALSE);
#endif
	m_isDeviceTestEnablePlayer = false; 
	int retCode = m_sdkWrapper.DeviceTestClosePlayer();		
	if(retCode == AV_OK)
	{
		uint32 playerVolume = 0;
		CString strPlayerVolume = _T("");
		strPlayerVolume.Format(_T("%d"), playerVolume);
		m_sliderDeviceTestAudioPlayerVolume.SetPos(playerVolume);
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnDeviceTestClosePlayer.EnableWindow(TRUE);
#endif
		m_isDeviceTestEnablePlayer = true;
	}

	ShowRetCode("DeviceTestClosePlayer", retCode);
}


void DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestCameraList()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

	int selIdx = m_comboDeviceTestCameraList.GetCurSel();
	if(m_sdkWrapper.DeviceTestGetSelectedCameraId() == m_deviceTestCameraList[selIdx].first)return;
	m_sdkWrapper.DeviceTestSetSelectedCameraId(m_deviceTestCameraList[selIdx].first);
	OnBnClickedBtnDeviceTestOpenCamera();
}


void DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestMicList()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}

	int selIdx = m_comboDeviceTestMicList.GetCurSel();
	if(m_sdkWrapper.DeviceTestGetSelectedMicId() == m_deviceTestMicList[selIdx].first)return;
	m_sdkWrapper.DeviceTestSetSelectedMicId(m_deviceTestMicList[selIdx].first);
	OnBnClickedBtnDeviceTestOpenMic();
}


void DialogQAVSDKDemo::OnCbnSelchangeComboDeviceTestPlayerList()
{
	if(!m_isStartDeviceTest)
	{
		ShowMessageBox(_T("不在设备测试中，无法进行操作！"));
		return;
	}
	
	int selIdx = m_comboDeviceTestPlayerList.GetCurSel();
	if(m_sdkWrapper.DeviceTestGetSelectedPlayerId() == m_deviceTestPlayerList[selIdx].first)return;
	m_sdkWrapper.DeviceTestSetSelectedPlayerId(m_deviceTestPlayerList[selIdx].first);
	OnBnClickedBtnDeviceTestOpenPlayer();
}

void DialogQAVSDKDemo::OnBnClickedBtnOpenScreenShareSend()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnOpenScreenShareSend.EnableWindow(FALSE);
#endif
	m_isEnableScreenShareSend = true;
	m_viewScreenShareRender.m_identifier = m_config.identifier;
	m_viewScreenShareRender.m_videoSrcType = VIDEO_SRC_TYPE_SCREEN;
		
	CString strLeft = _T("");
	m_editScreenShareCaptureLeft.GetWindowTextW(strLeft);
	uint32 x0 = _ttoi(strLeft);

	CString strTop = _T("");
	m_editScreenShareCaptureTop.GetWindowTextW(strTop);
	uint32 y0 = _ttoi(strTop);

	CString strRight = _T("");
	m_editScreenShareCaptureRight.GetWindowTextW(strRight);
	uint32 x1 = _ttoi(strRight);

	CString strBottom = _T("");
	m_editScreenShareCaptureBottom.GetWindowTextW(strBottom);
	uint32 y1 = _ttoi(strBottom);

	CString strFps = _T("");
	m_editScreenShareCaptureFps.GetWindowTextW(strFps);
	uint32 fps = _ttoi(strFps);

	int retCode = m_sdkWrapper.OpenScreenShareSend(x0, y0, x1, y1, fps);			
	if(retCode == AV_OK)
	{
		int selIdx = m_comboScreenShareModeList.GetCurSel();
		m_sdkWrapper.EnableScreenShareHDMode(selIdx == 0);

		uint32 newX0 = 0, newY0 = 0, newX1 = 0, newY1 = 0, newFps = 0;
		m_sdkWrapper.GetScreenCaptureParam(newX0, newY0, newX1, newY1, newFps);

		if(newX0 != x0 || newY0 != y0 || newX1 != x1 || newX0 != x0 || newY1 != y1 || newFps != fps)
		{
			CString strLeft = _T("");
			CString strTop = _T("");
			CString strRight = _T("");
			CString strBottom = _T("");
			CString strFps = _T("");

			strLeft.Format(_T("%u"), newX0);
			strTop.Format(_T("%u"), newY0);
			strRight.Format(_T("%u"), newX1);
			strBottom.Format(_T("%u"), newY1);
			strFps.Format(_T("%u"), newFps);

			m_editScreenShareCaptureLeft.SetWindowTextW(strLeft);
			m_editScreenShareCaptureTop.SetWindowTextW(strTop);
			m_editScreenShareCaptureRight.SetWindowTextW(strRight);
			m_editScreenShareCaptureBottom.SetWindowTextW(strBottom);
			m_editScreenShareCaptureFps.SetWindowTextW(strFps);		

			ShowMessageBox(_T("注意！所要捕获的画面：\r\n. 最大不能超过整个屏幕；\r\n. 宽度最小不能小于16，高度最小不能小于4；\r\n. 并且宽度必须为16的倍数，高度必需为2的倍数；\r\n. 捕获帧率取值范围[1-10]；\r\n否则SDK内部也会自动修正。"));
		}		
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnOpenScreenShareSend.EnableWindow(TRUE);
#endif
		m_isEnableScreenShareSend = false;
		m_viewScreenShareRender.m_identifier = "";
	}

	ShowRetCode("OpenScreenShareSend", retCode);
}


void DialogQAVSDKDemo::OnBnClickedBtnCloseScreenShareSend()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	if(m_viewScreenShareRender.m_identifier == m_config.identifier)
	{
		m_viewScreenShareRender.Clear();//如果显示着自己的画面，则应该清除。
	}

	if(m_viewBigVideoRender.m_identifier == m_config.identifier 
		&& m_viewBigVideoRender.m_videoSrcType == VIDEO_SRC_TYPE_SCREEN) 
	{
		m_viewBigVideoRender.Clear();
		UpadateVideoRenderInfo();
	}

#ifdef ENABLE_UI_OPERATION_SAFETY
	m_btnCloseScreenShareSend.EnableWindow(FALSE);
#endif
	m_isEnableScreenShareSend = false; 
	int retCode = m_sdkWrapper.CloseScreenShareSend();		
	if(retCode == AV_OK)
	{	
	}
	else
	{
#ifdef ENABLE_UI_OPERATION_SAFETY
		m_btnCloseScreenShareSend.EnableWindow(TRUE);
#endif
		m_isEnableScreenShareSend = true;
	}

	ShowRetCode("CloseScreenShareSend", retCode);
}

void DialogQAVSDKDemo::OnCbnSelchangeComboScreenShareModeList()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}

	int selIdx = m_comboScreenShareModeList.GetCurSel();
	m_sdkWrapper.EnableScreenShareHDMode(selIdx == 0);
}

void DialogQAVSDKDemo::Open1v1View()
{
	if(m_p1v1VideoRenderDlg == NULL)
	{
		m_p1v1VideoRenderDlg = new Dialog1v1VideoRender();
		m_p1v1VideoRenderDlg->Create(IDD_DIALOG_1V1_VIDEO_RENDER, this);
		m_p1v1VideoRenderDlg->ShowWindow(SW_SHOW | SW_NORMAL);
		m_sdkWrapper.Set1v1VideoRenderDlg(m_p1v1VideoRenderDlg);
	}
	else
	{
		m_p1v1VideoRenderDlg->ShowWindow(SW_SHOW | SW_NORMAL);
	}
}

void DialogQAVSDKDemo::Close1v1View()
{
	if (m_p1v1VideoRenderDlg)
	{
		m_sdkWrapper.Set1v1VideoRenderDlg(NULL);
		delete m_p1v1VideoRenderDlg;
		m_p1v1VideoRenderDlg = NULL;
	}
}

void DialogQAVSDKDemo::OnBnClickedBtnOpen1v1View()
{
	if(!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}	
	
	Open1v1View();
}


void DialogQAVSDKDemo::OnBnClickedBtnClose1v1View()
{
	Close1v1View();
}

void DialogQAVSDKDemo::UpdateVideoPushBtnsState()
{
	const wchar_t* PUSH_REQ_BTN_START_TXT = _T("推流(开始)");
	const wchar_t* PUSH_REQ_BTN_STOP_TXT = _T("推流(停止)");
	const wchar_t* RECORD_REQ_BTN_START_TXT = _T("录制(开始)");
	const wchar_t* RECORD_REQ_BTN_STOP_TXT = _T("录制(停止)");
	int comboIndex = m_comboPushVideoDataType.GetCurSel();
	if (comboIndex == -1) m_comboPushVideoDataType.SetCurSel(0);
	comboIndex = m_comboPushVideoEncodeType.GetCurSel();
	if (comboIndex == -1) m_comboPushVideoEncodeType.SetCurSel(0);
	comboIndex = m_comboRecordVideoDataType.GetCurSel();
	if (comboIndex == -1) m_comboRecordVideoDataType.SetCurSel(0);
	if (m_isEnterRoom)
	{
		m_btnPushVideoReq.SetWindowTextW(!m_isVideoPushing ? PUSH_REQ_BTN_START_TXT : PUSH_REQ_BTN_STOP_TXT);
		m_btnPushVideoReq.EnableWindow(TRUE);
		m_btnRecordVideoReq.SetWindowTextW(!m_isVideoRecording ? RECORD_REQ_BTN_START_TXT : RECORD_REQ_BTN_STOP_TXT);
		m_btnRecordVideoReq.EnableWindow(TRUE);
	}
	else
	{
		m_btnPushVideoReq.EnableWindow(FALSE);
		m_btnRecordVideoReq.EnableWindow(FALSE);
	}
}


void DialogQAVSDKDemo::OnClickedBtnPushVideoReq()
{
	if (!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
	
	//这五行自己加的
	CString channelName;
	m_comboChannelList.GetWindowText(channelName);
    ChannelInfo info = ConfigInfoMgr::GetInst()->GetChannelInfoById(channelName);
	
	CString channelDescription;
	channelDescription = info.channelDescription;
	 
	TIMRoomInfo roomInfo;
	roomInfo.relation_id = m_sdkWrapper.GetRoomInfo()->relation_id;
	roomInfo.room_id = (int)m_sdkWrapper.GetRoomInfo()->room_id;

	TIMStreamParam streaParam;
	int comboIndex = m_comboPushVideoDataType.GetCurSel();

	switch (comboIndex)
	{
	case 0:
		streaParam.push_data_type = 0;break;
	case 1:
		streaParam.push_data_type = 1;break;
	default:
		ShowMessageBox(_T("请选择推流数据源"));
		return;
	}

	comboIndex = m_comboPushVideoEncodeType.GetCurSel();
	switch (comboIndex)
	{
	case 0:
		streaParam.encode = HLS;break;
	case 1:
		streaParam.encode = RTMP;break;
	default:
		ShowMessageBox(_T("请选择编码类型"));
		return;
	}
	//const char* TEST_ROOM_NAME_STR = "TEST";
	streaParam.channel_name = StrWToStrA(channelName);  //参考后面，CString转string
	streaParam.channel_desc = StrWToStrA(channelDescription);
	if (!m_isVideoPushing)
	{
		TIMIntManager::get().RequestMultiVideoStreamerStart(roomInfo, streaParam, &m_pushVideoStartCallBack);  //旁路直播
		return;
	}
	else
	{
		std::list<uint64_t> channel_ids;
		channel_ids.push_back(m_uPushVideoChID);
		TIMIntManager::get().RequestMultiVideoStreamerStop(roomInfo, streaParam, channel_ids, &m_pushVideoStopCallBack);
	}
}


void DialogQAVSDKDemo::OnClickedBtnRecordVideoReq()  //录制
{
	if (!m_isEnterRoom)
	{
		ShowMessageBox(_T("不在房间中，无法进行操作！"));
		return;
	}
	TIMRoomInfo roomInfo;
	roomInfo.relation_id = m_sdkWrapper.GetRoomInfo()->relation_id;
	roomInfo.room_id = (int)m_sdkWrapper.GetRoomInfo()->room_id;
	TIMRecordParam recodePara;
	int comboIndex = m_comboPushVideoDataType.GetCurSel();
	switch (comboIndex)
	{
	case 0:
		recodePara.record_data_type = 0;break;
	case 1:
		recodePara.record_data_type = 1;break;
	default:
		ShowMessageBox(_T("请选择录制数据源"));
		return;
	}

	const char* TEST_ROOM_NAME_STR = "TEST";
	recodePara.filename = TEST_ROOM_NAME_STR;
	recodePara.class_id = 8921;

	if (!m_isVideoRecording)
	{
		TIMIntManager::get().RequestMultiVideoRecorderStart(roomInfo, recodePara, &m_recordVideoStartCallBack);
		return;
	}
	else
	{
		TIMIntManager::get().RequestMultiVideoRecorderStop(roomInfo, recodePara, &m_recordVideoStopCallBack);
	}
}

void DialogQAVSDKDemo::OnBnClickedButtonSearchPara()  //参数输出
{
	// TODO: Add your control notification handler code here
	CString cstrfilepath; 
	std::string strPrivilegeMap;
	para_file_path.GetWindowText(cstrfilepath);

	std::string strfilepath = StrWToStrA(cstrfilepath);
	strfilepath.append("\\paras.txt");
	
	if (!strfilepath.size())
	{
		::MessageBox(0, L"请填写参数文件输出路径", L"查询参数输出", 0);
	}

	else
	{
		std:string paras;
		paras = m_sdkWrapper.GetRoomStatParam();
		FILE* para_file = fopen(strfilepath.c_str(),"ab+");
		
		fprintf(para_file,"\n\n%s",paras.c_str());
	
		fclose(para_file);
	}
}


void DialogQAVSDKDemo::OnStnClickedViewScreenShare()
{
	// TODO: 在此添加控件通知处理程序代码
}


void DialogQAVSDKDemo::OnBnClickedCheckIsAutoCreateSdkRoom()
{
	// TODO: 在此添加控件通知处理程序代码
}


void DialogQAVSDKDemo::OnStnClickedStaticRoomId()
{
	// TODO: 在此添加控件通知处理程序代码
}


void DialogQAVSDKDemo::OnStnClickedStaticCamera()
{
	// TODO: 在此添加控件通知处理程序代码
}
