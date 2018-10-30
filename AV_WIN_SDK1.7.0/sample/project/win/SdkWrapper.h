#pragma once
#include "av_sdk.h"                          //外部依赖项
#include <vector>
#include <map>
#include <fstream>
#include "Util.h"
#ifdef TEST_FOR_INTERNAL
#include "include_for_internal\av_context_internal.h"
#endif

using namespace std;
using namespace tencent::av;

#define VIDEO_RENDER_BIG_VIEW_WIDTH 640  //这里改了，原先是640，改了没作用？
#define VIDEO_RENDER_BIG_VIEW_HEIGHT 480  //这里改了，原先是480，改了没作用？
#define VIDEO_RENDER_SMALL_VIEW_WIDTH 160
#define VIDEO_RENDER_SMALL_VIEW_HEIGHT 120

class SdkWrapper:public AVRoomMulti::Delegate                //多人音视频房间类
{
public:
	SdkWrapper(void* pRenderDlg);
	~SdkWrapper(void);

  std::string GetSDKVersion();
  void EnableCrashReport(bool enable);

	int CreateContext(const AVContext::Config& config);
	int DestroyContext();
	int StartContext();
	int StopContext();

	//房间
	int EnterRoom(AVRoomMulti::EnterRoomParam& param);
	int ExitRoom();
	int ChangeAuthority(uint64 authBits,const std::string& strPrivilegeMap);
	//AVEndpoint* GetEndpointByIndex(int index);
	AVEndpoint* GetEndpointById(string identifier);
	int GetEndpointList(AVEndpoint** endpointList[]);
	const AVRoom::Info* GetRoomInfo();

	//房间成员
	int MuteAudio(std::string identifier, bool is_mute = false);	
	int RequestViewList(std::vector<std::string> identifierList, std::vector<View> viewList);
	int CancelAllView();

	//语音控制
	bool EnableBoost(bool isEnable);
	bool IsBoostEnable();
	bool EnableNS(bool isEnable);
	bool IsNSEnable();
	bool EnableAEC(bool isEnable);
	bool IsAECEnable();

	//语音设备
	//麦克风
	int OpenMic();
	int CloseMic();
	int SetSelectedMicId(std::string micId);
	std::string GetSelectedMicId();
	int GetMicList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &micList);
	void SetMicVolume(uint32 value);
	uint32 GetMicVolume();

	//播放器(扬声器/耳机)
	int OpenPlayer();
	int ClosePlayer();
	int SetSelectedPlayerId(std::string playerId);
	std::string GetSelectedPlayerId();
	int GetPlayerList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &playerList);
	void SetPlayerVolume(uint32 value);
	uint32 GetPlayerVolume();

	//伴奏
	int StartAccompany(std::string playerPath, std::string mediaFilePath, AVAccompanyDevice::SourceType sourceType);
	int StopAccompany();
	void SetAccompanyVolume(uint32 value);
	uint32 GetAccompanyVolume();

	//输入混音
	int StartMicAndAccompanyForMix(std::vector<std::string> localDeviceList);
	int StopMicAndAccompanyForMix(std::vector<std::string> localDeviceList);

	//视频设备
	//摄像头
	int OpenCamera();
	int CloseCamera();
	int SetSelectedCameraId(std::string cameraId);
	std::string GetSelectedCameraId();
	int GetCameraList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &cameraList);

	//屏幕分享
	int OpenScreenShareSend(uint32 left, uint32 top, uint32 right, uint32 bottom, uint32 fps);
	int CloseScreenShareSend();
	void GetScreenCaptureParam(uint32 &left, uint32 &top, uint32 &right, uint32 &bottom, uint32 &fps);
	void EnableScreenShareHDMode(bool isEnable);

	int SetExternalCamAbility(CameraInfo* pCamInfo);
	int OpenExternalCapture();
	int CloseExternalCapture();
	int FillExternalCaptureFrame(VideoFrame* pFrame);

	void SetVideoParam(AVSupportVideoPreview::PreviewParam);
	void ClearVideoParam();


	//设备管理
	void GetAudioInputDeviceList();

	//其他
	std::string GetQualityTips();

	//查询运行时参数
	std::string GetRoomStatParam();

	void UpdateRequestViewIdentifierList(std::vector<std::string> identifierList){m_curRequestViewIdentifierList = identifierList;/*TODO 这边未加保护，可能会导致使用到该变量的逻辑异常*/}
	std::vector<std::string> GetRequestViewIdentifierList(){return m_curRequestViewIdentifierList;}
	void UpdateRequestViewParamList(std::vector<View> viewList){m_curRequestViewParamList = viewList;/*TODO 这边未加保护，可能会导致使用到该变量的逻辑异常*/}
	std::vector<View> GetRequestViewParamList(){return m_curRequestViewParamList;}
	void SetDefaultCameraState(bool isEnable){m_isSelectedCameraEnable = isEnable;};
	bool GetDefaultCameraState(){return m_isSelectedCameraEnable;};
	void SetDefaultScreenShareSendState(bool isEnable){m_isSelectedScreenShareSendEnable = isEnable;};
	bool GetDefaultScreenShareSendState(){return m_isSelectedScreenShareSendEnable;};
	void SetDefaultMicState(bool isEnable){m_isSelectedMicEnable = isEnable;};
	bool GetDefaultMicState(){return m_isSelectedMicEnable;};
	void SetDefaultPlayerState(bool isEnable){m_isSelectedPlayerEnable = isEnable;};
	bool GetDefaultPlayerState(){return m_isSelectedPlayerEnable;};

#ifdef TEST_FOR_INTERNAL
    bool SetUDTParam(SpeciUDTParam* udt_param);
#endif
    void EnableHostMode();
public:
	
	virtual void OnEnterRoomComplete(int result);
	virtual void OnExitRoomComplete(int result);
	virtual void OnChangeAuthority(int32 ret_code);

	//virtual void OnEndpointsEnterRoom(int endpointCount, AVEndpoint *endPointList[]);
	//virtual void OnEndpointsExitRoom(int endpointCount, AVEndpoint *endPointList[]);
	virtual void OnEndpointsUpdateInfo(AVRoom::EndpointEventId eventid, std::vector<std::string> updatelist);
	virtual void OnEndpointsUpdateInfo();
    virtual void OnPrivilegeDiffNotify(int32 privilege);
	virtual void OnDeviveDetecNotify(AVDeviceMgr* device_mgr,DetectedDeviceInfo& info, bool*pbUnselect, void* custom_data);

	//DeviceTest
	int StartDeviceTest();
	int StopDeviceTest();
	void DeviceTestSetSelectedMicId(std::string deviceId);
	std::string DeviceTestGetSelectedMicId(){return m_deviceTestSelectedMicId;}
	int DeviceTestOpenMic();
	int DeviceTestCloseMic();
	int DeviceTestGetMicList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceId);
	void DeviceTestSetMicVolume(uint32 value);
	uint32 DeviceTestGetMicVolume();
	uint32 DeviceTestGetMicDynamicVolume();
	
	void DeviceTestSetSelectedPlayerId(std::string deviceId);
	std::string DeviceTestGetSelectedPlayerId(){return m_deviceTestSelectedPlayerId;}
	int DeviceTestOpenPlayer();
	int DeviceTestClosePlayer();
	int DeviceTestGetPlayerList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceId);
	void DeviceTestSetPlayerVolume(uint32 value);
	uint32 DeviceTestGetPlayerVolume();
	uint32 DeviceTestGetPlayerDynamicVolume();
	
	void DeviceTestSetSelectedCameraId(std::string deviceId);
	std::string DeviceTestGetSelectedCameraId(){return m_deviceTestSelectedCameraId;}
	int DeviceTestOpenCamera();
	int DeviceTestCloseCamera();
	int DeviceTestGetCameraList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceId);

	static void OnStartDeviceTestCompleteCallback(int result, void *pCustomData);
	static void OnStopDeviceTestCompleteCallback(void *pCustomData);
	static void OnDeviceTestLocalVideo(VideoFrame *pFrameData, void *pCustomData);
	static void OnDeviceTestDeviceOperationCallback(AVDeviceTest *pDevTest, AVDevice::DeviceOperation oper, const std::string &deviceId, int retCode, void *pCustomData);
	
	int StartInputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type, std::string path);
	int StopInputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type);

	int StartOutputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type);
	int StopOutputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type);

	int EnableAudioData(AVAudioCtrl::AudioDataSourceType src_type, bool enable);
	bool IsEnableAudioData(AVAudioCtrl::AudioDataSourceType src_type);

	int SetAudioDataFormat(AVAudioCtrl::AudioDataSourceType src_type, AudioFrameDesc audio_desc);
	int GetAudioDataFormat(AVAudioCtrl::AudioDataSourceType src_type, AudioFrameDesc& audio_desc);
	int SetAudioDataVolume(AVAudioCtrl::AudioDataSourceType src_type, float volume);
	int GetAudioDataVolume(AVAudioCtrl::AudioDataSourceType src_type, float* volume);

	void Set1v1VideoRenderDlg(void *p1v1VideoRenderDlg);

private:
	int AudioDataCallback(AudioFrame* audio_frame, AVAudioCtrl::AudioDataSourceType src_type);
	static int AudioDataCallback(AudioFrame* audio_frame, AVAudioCtrl::AudioDataSourceType src_type, void* custom_data);

private:
	void OnStartDeviceTestCompleteCallbackInternal(int result);
	void OnStopDeviceTestCompleteCallbackInternal();

private:
	void OnStopContextCompleteCallbackInternal();

protected:
	static void OnStartContextCompleteCallback(int result, void *pCustomData);
	static void OnStopContextCompleteCallback(void *pCustomData);

	static void OnRequestViewListCompleteCallback(std::vector<std::string> identifierList, std::vector<View> viewList, int32 result, void *pCustomData);
	static void OnCancelAllViewCompleteCallback(int result, void *pCustomData);
	static void OnAudioDeviceOperationCallback(AVDeviceMgr *pAudMgr, AVDevice::DeviceOperation oper, const std::string &deviceId, int retCode, void *pCustomData);
	static void OnVideoDeviceOperationCallback(AVDeviceMgr *pVidMgr, AVDevice::DeviceOperation oper, const std::string &deviceId, int retCode, void *pCustomData);
	static void OnAudioDeviveDetectNotify(AVDeviceMgr* device_mgr,DetectedDeviceInfo& info, bool*pbUnselect, void* custom_data);
	static void OnRemoteVideo(VideoFrame *pFrameData, void *pCustomData);
	static void OnLocalVideo(VideoFrame *pFrameData, void *pCustomData);
	static void OnPreTreatmentFun(VideoFrame *pFrameData, void *pCustomData);
	static void OnChangeAuthorityCallback(int32 ret_code, void *pCustomData);

private:
	void *m_pMainDlg;	
	void *m_p1v1VideoRenderDlg;
#ifdef TEST_FOR_INTERNAL
    AVContextInternal *m_pContext;
#else
    AVContext *m_pContext;
#endif
	
	AVRoomMulti *m_pRoom;
	AVAudioCtrl *m_pAudCtrl;
	AVVideoCtrl *m_pVidCtrl;
	AVDeviceMgr *m_pAudMgr;
	AVDeviceMgr *m_pVidMgr;
	AVDeviceTest *m_pDeviceTest;

	int m_curSelectedCameraIdx;
	AVSupportVideoPreview::PreviewParam m_cameraParam;
	AVSupportVideoPreview::PreviewParam m_remoteVideoParam;
	AVSupportVideoPreview::PreviewParam m_screenSendParam;
	AVSupportVideoPreview::PreviewParam m_screenRecvParam;

	std::vector<std::string> m_curRequestViewIdentifierList;
	std::vector<View> m_curRequestViewParamList;

	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_micList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_playerList;
	std::vector<std::pair<std::string/*id*/, std::string/*name*/> > m_cameraList;
	std::string m_selectedMicId;
	std::string m_selectedPlayerId;
	std::string m_selectedCameraId;
	bool m_isSelectedMicEnable;
	bool m_isSelectedPlayerEnable;
	bool m_isSelectedCameraEnable;
	bool m_isSelectedScreenShareSendEnable;

	std::map<AVAudioCtrl::AudioDataSourceType, FILE*> m_mapAudioData;
	std::map<AVAudioCtrl::AudioDataSourceType, std::string> m_mapAudioDataName;
	std::map<std::string/*id*/, FILE*> m_mapAudioDataNetStreams;

	bool m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_END];
	AudioFrameDesc m_AudioDataDesc[AVAudioCtrl::AUDIO_DATA_SOURCE_END];
	SimpleLock m_AudioDataLock[AVAudioCtrl::AUDIO_DATA_SOURCE_END];
	SimpleLock m_AudioDataMapLock;

	std::string m_strMixToSendPath;
	std::string m_strMixToPlayPath;

	HMODULE m_hMoude;                                   //模块句柄
	PROC_AVAPI_GetSDKVersion m_pGetSDKVersion;
	PROC_AVAPI_CreateContext m_pCreateContext;
	PROC_AVAPI_DestroyContext m_pDestroyContext;
	PROC_AVAPI_EnableCrashReport m_pEnableCrashReport;

	//DeviceTest
	std::string m_deviceTestSelectedMicId;
	std::string m_deviceTestSelectedPlayerId;
	std::string m_deviceTestSelectedCameraId;
};

