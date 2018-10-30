#include "StdAfx.h"
#include "SdkWrapper.h"
#include "Util.h"
#include "CustomWinMsg.h"
#include "DialogQAVSDKDemo.h"
#include "Dialog1v1VideoRender.h"
#include <iostream>

SdkWrapper::SdkWrapper(void* pMainDlg)
{
	m_pMainDlg = pMainDlg;
	m_p1v1VideoRenderDlg = NULL;

	m_pContext = NULL;
	m_pRoom = NULL;
	m_pAudCtrl = NULL;
	m_pVidCtrl = NULL;
	m_pAudMgr = NULL;
	m_pVidMgr = NULL;
	m_pDeviceTest = NULL;
	//m_curSelectedCameraIdx = -1;//����δ������ͷ
	
	m_cameraParam.device_id = "";/*��ָ��id������ÿ·��Ƶ�������������;��Ϊ��ʱ�������豸��AVRemoteVideoDevice����device_id����ΪԶ���豸��Ӧ�ĳ�Աidentifier*/
	m_cameraParam.width = 0;//Ԥ�������ȣ�Ϊ0��������Ƶͼ���Ĭ�Ͽ��VIDEO_RENDER_BIG_VIEW_WIDTH;
	m_cameraParam.height = 0;//VIDEO_RENDER_BIG_VIEW_HEIGHT;
	m_cameraParam.color_format = COLOR_FORMAT_RGB24;//Ԥ����Ƶͼ�����ɫ��ʽ��SDK1.3�汾ֻ֧��COLOR_FORMAT_RGB24��COLOR_FORMAT_I420
	m_cameraParam.src_type = VIDEO_SRC_TYPE_CAMERA;//��ƵԴ���ͣ�SDK1.3�汾ֻ֧��VIDEO_SRC_TYPE_CAMERA

	m_remoteVideoParam.device_id = "";/*��ָ��id������ÿ·��Ƶ�������������*/
	m_remoteVideoParam.width = 0;//VIDEO_RENDER_BIG_VIEW_WIDTH;
	m_remoteVideoParam.height = 0;//VIDEO_RENDER_BIG_VIEW_HEIGHT;
	m_remoteVideoParam.color_format = COLOR_FORMAT_RGB24;//SDK1.3�汾ֻ֧��COLOR_FORMAT_RGB24��COLOR_FORMAT_I420
	m_remoteVideoParam.src_type = VIDEO_SRC_TYPE_CAMERA;//SDK1.3�汾ֻ֧��VIDEO_SRC_TYPE_CAMERA

	m_screenSendParam.device_id = "";/*��ָ��id������ÿ·��Ƶ�������������*/
	m_screenSendParam.width = 0;//VIDEO_RENDER_BIG_VIEW_WIDTH;
	m_screenSendParam.height = 0;//VIDEO_RENDER_BIG_VIEW_HEIGHT;
	m_screenSendParam.color_format = COLOR_FORMAT_RGB24;//SDK1.6�汾ֻ֧��COLOR_FORMAT_RGB24��COLOR_FORMAT_I420
	m_screenSendParam.src_type = VIDEO_SRC_TYPE_SCREEN;//SDK1.6�汾ֻ֧��VIDEO_SRC_TYPE_SCREEN

	m_screenRecvParam.device_id = "";/*��ָ��id������ÿ·��Ƶ�������������*/
	m_screenRecvParam.width = 0;//VIDEO_RENDER_BIG_VIEW_WIDTH;
	m_screenRecvParam.height = 0;//VIDEO_RENDER_BIG_VIEW_HEIGHT;
	m_screenRecvParam.color_format = COLOR_FORMAT_RGB24;//SDK1.6�汾ֻ֧��COLOR_FORMAT_RGB24��COLOR_FORMAT_I420
	m_screenRecvParam.src_type = VIDEO_SRC_TYPE_SCREEN;//SDK1.6�汾ֻ֧��VIDEO_SRC_TYPE_SCREEN

	m_selectedMicId = "";
	m_selectedPlayerId = "";
	m_selectedCameraId = "";
	m_isSelectedMicEnable = false;
	m_isSelectedPlayerEnable = false;
	m_isSelectedCameraEnable = false;
	m_isSelectedScreenShareSendEnable = false;

	m_deviceTestSelectedMicId = "";
	m_deviceTestSelectedPlayerId = "";
	m_deviceTestSelectedCameraId = "";

	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_MIC] = false;
	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND] = false;
	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_SEND] = false;
	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY] = false;
	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY] = false;
	m_mapAudioDataEnable[AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM] = false;

	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_MIC] = "Mic";
	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND] = "MixToSend";//����������Ƶ���ݣ��뱾�ط��͵���Ƶ���ݻ������ͳ�ȥ
	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_SEND] = "Send";//���ͷ����շ��ͳ�ȥ����Ƶ����
	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY] = "MixToPlay";//����������Ƶ���ݣ��뱾�ز��ŵ���Ƶ���ݻ���������������ų���
	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_PLAY] = "Play";//��ȡ����������������Ƶ����
	m_mapAudioDataName[AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM] = "NetStream";///���շ��յ�����Ƶ����

	m_hMoude = ::LoadLibrary(_T("qavsdk.dll"));                       //ģ����
	m_pGetSDKVersion = (PROC_AVAPI_GetSDKVersion)::GetProcAddress(m_hMoude, "AVAPI_GetSDKVersion"/*������*/);
	m_pCreateContext = (PROC_AVAPI_CreateContext)::GetProcAddress(m_hMoude, "AVAPI_CreateContext");
	m_pDestroyContext = (PROC_AVAPI_DestroyContext)::GetProcAddress(m_hMoude, "AVAPI_DestroyContext");
	m_pEnableCrashReport = (PROC_AVAPI_EnableCrashReport)::GetProcAddress(m_hMoude, "AVAPI_EnableCrashReport");
}

SdkWrapper::~SdkWrapper(void)
{
	if(m_pContext && m_pDestroyContext)
	{
		(*m_pDestroyContext)((void*)m_pContext);
		m_pContext = NULL;
	}	
	//if(m_hMoude)::FreeLibrary(m_hMoude);
}

std::string SdkWrapper::GetSDKVersion()
{
  if(!m_hMoude) return "Unknown";
  if(!m_pGetSDKVersion) return "Unknown";

  return (*m_pGetSDKVersion)();
}

void SdkWrapper::EnableCrashReport(bool enable)
{
  return (*m_pEnableCrashReport)(enable);
}

int SdkWrapper::CreateContext(const AVContext::Config& config)
{
	if(m_pContext == NULL)
	{
		AVContext* pContext = (AVContext*)(*m_pCreateContext)((void*)&config); 
#ifdef TEST_FOR_INTERNAL       //TEST_FOR_INTERNALδ�����壬����ִ��else����Ĵ���
		m_pContext = dynamic_cast<AVContextInternal*>(pContext);
#else
		m_pContext = pContext;
#endif
	}
	return AV_OK;
}

int SdkWrapper::DestroyContext()
{
	if(m_pContext && m_pDestroyContext)
	{
		(*m_pDestroyContext)((void*)m_pContext);
		m_pContext = NULL;
	}
	return AV_OK;
}

int SdkWrapper::StartContext()
{
	if(!m_hMoude) return AV_ERR_FAILED;  //һ�����
	if(!m_pGetSDKVersion) return AV_ERR_FAILED;
	if(!m_pCreateContext) return AV_ERR_FAILED;
	if(!m_pDestroyContext) return AV_ERR_FAILED;
	if(!m_pEnableCrashReport) return AV_ERR_FAILED;

	if(m_pContext == NULL)return AV_ERR_FAILED;	

	return m_pContext->StartContext(&SdkWrapper::OnStartContextCompleteCallback, this);  //����ָ�룬ָ��App����Ļص�������Appָ����һ��û�����͵�ָ�룬SDK���ڻص������аѸ�ֵ�ش���App
}

int SdkWrapper::StopContext()
{
	if(m_pContext == NULL)return AV_ERR_FAILED;		
	return m_pContext->StopContext(OnStopContextCompleteCallback, this);
}

//����
int SdkWrapper::EnterRoom(AVRoomMulti::EnterRoomParam& param)
{	
	if(m_pContext == NULL)return AV_ERR_FAILED;	
	return m_pContext->EnterRoom(this, &param);
}

int SdkWrapper::ExitRoom()
{
	if(m_pContext == NULL)return AV_ERR_FAILED;	
	return m_pContext->ExitRoom();
}

int SdkWrapper::ChangeAuthority(uint64 authBits,const std::string& strPrivilegeMap)//�޸�ͨ������Ȩ��
{
  if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)/*��һΪNULLʱ*/return NULL;
//	if(strPrivilegeMap == "") return NULL;		
	return m_pRoom->ChangeAuthority(authBits, strPrivilegeMap, SdkWrapper::OnChangeAuthorityCallback, this);
}

//AVEndpoint* SdkWrapper::GetEndpointByIndex(int index)
//{
//	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return NULL;		
//	return m_pRoom->GetEndpointByIndex(index);
//}

AVEndpoint* SdkWrapper::GetEndpointById(string identifier)//��ȡ�����Ա����
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return NULL;
	if(identifier == "") return NULL;	
	return m_pRoom->GetEndpointById(identifier);
}

int SdkWrapper::GetEndpointList(AVEndpoint** endpointList[])
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return 0;
	return m_pRoom->GetEndpointList(endpointList);
}

//�����Ա
int SdkWrapper::MuteAudio(std::string identifier, bool is_mute) 
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	if(identifier == "") return AV_ERR_FAILED;	

	AVEndpoint *pEndpoint = m_pRoom->GetEndpointById(identifier);
	if (pEndpoint)
	{
		bool ret = pEndpoint->MuteAudio(is_mute);
		return ret ? AV_OK : AV_ERR_FAILED;
	}
	else
	{
		return AV_ERR_FAILED;
	}

	return AV_OK;
}

/*
RequestViewList����ͬʱ�����·���档���Ҫͬһʱ�̿�����·���棬����ʹ�øýӿڡ�
ע�⣺
. ÿ�������·����ʱ�����붼������Ҫ�鿴�ĳ�Ա��id�����һ��Ҫͬʱ��A,B��C�Ļ��棬�ͱ�������������˵�id��
	�������ȥҪͬʱ��A��B�Ļ��棬�ͱ�������������˵�id���������ȥ��Ҫͬʱ��A,B,C��D�Ļ��棬�ͱ���������ĸ��˵�id��
*/
int SdkWrapper::RequestViewList(std::vector<std::string> identifierList, std::vector<View> viewList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	if(identifierList.size() == 0 || identifierList.size() != viewList.size()) return AV_ERR_INVALID_ARGUMENT;	//�������

	/*
	ע��:
	1. �����С���Ը���ҵ���ʵ����Ҫ��Ӳ������������
	2. ������ֻ�������ֻ������һ·�Ǵ��棬��������С���棬����Ӳ�������׿���ס��ͬʱ����������
	3. ��߰�320��240�����ϴ�С�Ļ�����Ϊ�Ǵ��棻��֮����Ϊ��С���档
	4. ʵ�������󵽵Ļ����С���ɷ��ͷ���������A���Ļ�����С���棬��ʹ��߼�ʹ���������Ĵ��棬Ҳֻ�����󵽵�С���档
	5. ���ͷ����Ļ����С���Ƿ�ͬʱ�д����С���棬���������õı���������������Ӳ������������ؾ�����
	*/

	return m_pRoom->RequestViewList(identifierList, viewList, OnRequestViewListCompleteCallback, this);
}

/*
CancelAllView����ȡ�����еĻ��档
*/
int SdkWrapper::CancelAllView()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	if(m_curRequestViewIdentifierList.size() == 0)
	{
		return AV_ERR_FAILED;
	}

	m_curRequestViewIdentifierList.clear();
	m_curRequestViewParamList.clear();
	return m_pRoom->CancelAllView(OnCancelAllViewCompleteCallback, this);	
}

//��������
bool SdkWrapper::EnableBoost(bool isEnable)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->EnableBoost(isEnable);
}

bool SdkWrapper::IsBoostEnable()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->IsBoostEnable();
}

bool SdkWrapper::EnableNS(bool isEnable)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->EnableNS(isEnable);
}

bool SdkWrapper::IsNSEnable()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->IsNSEnable();
}

bool SdkWrapper::EnableAEC(bool isEnable)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->EnableAEC(isEnable);
}

bool SdkWrapper::IsAECEnable()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return false;
	return m_pAudCtrl->IsAECEnable();
}

//��˷�
int SdkWrapper::OpenMic()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	
	if(m_selectedMicId == "")return AV_ERR_DEVICE_NOT_EXIST;//�豸������
	//if(m_isSelectedMicEnable)return AV_ERR_HAS_IN_THE_STATE;

	return m_pAudMgr->SelectInputDevice(m_selectedMicId, true);//ѡ�������豸

// 	AVDevice **ppMicList = NULL;
// 	int micCount = m_pAudMgr->GetDeviceByType(DEVICE_MIC, &ppMicList);//��ȡ��˷��б�
// 	if(ppMicList == NULL)return AV_ERR_DEVICE_NOT_EXIST;
  
// 	if(micCount == 0)
// 	{
// 		return AV_ERR_DEVICE_NOT_EXIST;
// 	}
// 	else if(micCount == 1)
// 	{
// 		int retCode = m_pAudMgr->SelectInputDevice(ppMicList[0]->GetId(), true);
// 		delete []ppMicList;
// 		return retCode;
// 	}
// 	else//�����˷�
// 	{
// 		//���Ĭ�ϲ�����һ���豸������ͬ���͵��豸�ǻ���ģ�ͬһ��ʱ��ֻ�п��Բ������������ֻ����һ����SDK�ڲ�Ĭ�ϻ�ر������豸��
// 		int retCode = m_pAudMgr->SelectInputDevice(ppMicList[0]->GetId(), true);
// 		delete []ppMicList;
// 		return retCode;
// 	}	
}

int SdkWrapper::CloseMic()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	
	if(m_selectedMicId == "")return AV_ERR_DEVICE_NOT_EXIST;
	return m_pAudMgr->SelectInputDevice(m_selectedMicId, false);//ȡ�������豸

// 	AVDevice **ppMicList = NULL;
// 	int micCount = m_pAudMgr->GetDeviceByType(DEVICE_MIC, &ppMicList);//��ȡ��˷��б�
// 	if(ppMicList == NULL)return AV_ERR_DEVICE_NOT_EXIST;
// 
// 	if(micCount == 0)
// 	{
// 		return AV_ERR_DEVICE_NOT_EXIST;
// 	}
// 	else if(micCount == 1)
// 	{
// 		int retCode = m_pAudMgr->SelectInputDevice(ppMicList[0]->GetId(), false);
// 		delete []ppMicList;
// 		return retCode;
// 	}
// 	else//�����˷�
// 	{
// 		//����ǰ��Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸
// 		int retCode = m_pAudMgr->SelectInputDevice(ppMicList[0]->GetId(), false);
// 		delete []ppMicList;
// 		return retCode;
// 	}	
}

int SdkWrapper::SetSelectedMicId(std::string micId)
{
	for(int i = 0; i < m_micList.size(); i++)
	{
		if(micId == m_micList[i].first)//first��micId��second��micName
		{
			m_selectedMicId = micId;
			return AV_OK;
		}
	}
	return AV_ERR_DEVICE_NOT_EXIST;
}

std::string SdkWrapper::GetSelectedMicId()
{
	return m_selectedMicId;
}

int SdkWrapper::GetMicList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &micList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	AVDevice **ppMicList = NULL;
	int micCount = m_pAudMgr->GetDeviceByType(DEVICE_MIC, &ppMicList);//��ȡ��˷��б�
	if(ppMicList == NULL)return AV_ERR_DEVICE_NOT_EXIST;
  
	if(micCount == 0)
	{
		return AV_ERR_DEVICE_NOT_EXIST;
	}

	for(int i = 0; i < micCount; i++)
	{
		std::pair<std::string/*id*/, std::string/*name*/> mic;
		mic.first = ppMicList[i]->GetId();
		mic.second = ppMicList[i]->GetInfo().name;
		micList.push_back(mic);
	}
	if(ppMicList)delete []ppMicList;//ҵ��ฺ���ͷ�������顣
	return AV_OK;
}

void SdkWrapper::SetMicVolume(uint32 value)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;
	
	AVDevice **ppMicList = NULL;
	int micCount = m_pAudMgr->GetDeviceByType(DEVICE_MIC, &ppMicList);//��ȡ��˷��б�
	if(ppMicList == NULL)return;

	if(micCount == 0)
	{
		return;
	}
	else if(micCount == 1)
	{
		((AVMicDevice *)ppMicList[0])->SetVolume(value);
		return;
	}
	else//�����˷�
	{		
		//����Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸��
		((AVMicDevice*)ppMicList[0])->SetVolume(value);
	}
	if(ppMicList)delete []ppMicList;//ҵ��ฺ���ͷ�������顣
}


uint32 SdkWrapper::GetMicVolume()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return MIN_AUDIO_DEVICE_VOLUME;
	
	AVDevice **ppMicList = NULL;
	int micCount = m_pAudMgr->GetDeviceByType(DEVICE_MIC, &ppMicList);//��ȡ��˷��б�
	if(ppMicList == NULL)return MIN_AUDIO_DEVICE_VOLUME;

	if(micCount == 0)
	{
		return MIN_AUDIO_DEVICE_VOLUME;
	}
	else if(micCount == 1)
	{
		uint32 volume = ((AVMicDevice *)ppMicList[0])->GetVolume();
		if(ppMicList)delete []ppMicList;//ҵ��ฺ���ͷ�������顣
		return volume;
	}
	else//�����˷�
	{
		//����Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸��		
		uint32 volume = ((AVMicDevice*)ppMicList[0])->GetVolume();	
		if(ppMicList)delete []ppMicList;//ҵ��ฺ���ͷ�������顣
		return volume;
	}	
}


//������
int SdkWrapper::OpenPlayer()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;	
	
	if(m_selectedPlayerId == "")return AV_ERR_DEVICE_NOT_EXIST;
	return m_pAudMgr->SelectOutputDevice(m_selectedPlayerId, true);

// 	AVDevice **ppPlayerList = NULL;
// 	int playerCount = m_pAudMgr->GetDeviceByType(DEVICE_PLAYER, &ppPlayerList);//��ȡ�������б�
// 	if(ppPlayerList == NULL)return AV_ERR_DEVICE_NOT_EXIST;
// 
// 	if(playerCount == 0)
// 	{
// 		return AV_ERR_DEVICE_NOT_EXIST;
// 	}
// 	else if(playerCount == 1)
// 	{
// 		int retCode = m_pAudMgr->SelectOutputDevice(ppPlayerList[0]->GetId(), true);
// 		delete []ppPlayerList;
// 		return retCode;
// 	}
// 	else//���������
// 	{		
// 		//���Ĭ�ϲ�����һ���豸������ͬ���͵��豸�ǻ���ģ�ͬһ��ʱ��ֻ�п��Բ������������ֻ����һ����SDK�ڲ�Ĭ�ϻ�ر������豸��
// 		int retCode = m_pAudMgr->SelectOutputDevice(ppPlayerList[0]->GetId(), true);//�򿪵�һ��������
// 		delete []ppPlayerList;
// 		return retCode;
// 	}	
}

int SdkWrapper::ClosePlayer()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	if(m_selectedPlayerId == "")return AV_ERR_DEVICE_NOT_EXIST;
	return m_pAudMgr->SelectOutputDevice(m_selectedPlayerId, false);

// 	AVDevice **ppPlayerList = NULL;
// 	int playerCount = m_pAudMgr->GetDeviceByType(DEVICE_PLAYER, &ppPlayerList);//��ȡ�������б�
// 	if(ppPlayerList == NULL)return AV_ERR_DEVICE_NOT_EXIST;
// 
// 	if(playerCount == 0)
// 	{
// 		return AV_ERR_DEVICE_NOT_EXIST;
// 	}
// 	else if(playerCount == 1)
// 	{
// 		int retCode = m_pAudMgr->SelectOutputDevice(ppPlayerList[0]->GetId(), false);
// 		delete []ppPlayerList;
// 		return retCode;
// 	}
// 	else//���������
// 	{
// 		//����ǰ��Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸
// 		int retCode = m_pAudMgr->SelectOutputDevice(ppPlayerList[0]->GetId(), false);		
// 		delete []ppPlayerList;
// 		return retCode;
// 	}	
}

int SdkWrapper::SetSelectedPlayerId(std::string playerId)
{
	for(int i = 0; i < m_playerList.size(); i++)
	{
		if(playerId == m_playerList[i].first)
		{
			m_selectedPlayerId = playerId;
			return AV_OK;
		}
	}

	return AV_ERR_DEVICE_NOT_EXIST;
}

std::string SdkWrapper::GetSelectedPlayerId()
{
	return m_selectedPlayerId;
}

int SdkWrapper::GetPlayerList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &playerList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	AVDevice **ppPlayerList = NULL;
	int playerCount = m_pAudMgr->GetDeviceByType(DEVICE_PLAYER, &ppPlayerList);//��ȡ�������б�
	if(ppPlayerList == NULL)return AV_ERR_DEVICE_NOT_EXIST;

	if(playerCount == 0)
	{
		return AV_ERR_DEVICE_NOT_EXIST;
	}

	for(int i = 0; i < playerCount; i++)
	{
		std::pair<std::string/*id*/, std::string/*name*/> player;
		player.first = ppPlayerList[i]->GetId();
		player.second = ppPlayerList[i]->GetInfo().name;
		playerList.push_back(player);//c.push_back(X) ��Ԫ��X���뵽c���������һλ
	}
	if(ppPlayerList)delete []ppPlayerList;//ҵ��ฺ���ͷ�������顣
	return AV_OK;
}

void SdkWrapper::SetPlayerVolume(uint32 value)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;
	
	AVDevice **ppPlayerList = NULL;
	int playerCount = m_pAudMgr->GetDeviceByType(DEVICE_PLAYER, &ppPlayerList);//��ȡ�������б�
	if(ppPlayerList == NULL)return;

	if(playerCount == 0)
	{
		return;
	}
	else if(playerCount == 1)
	{
		((AVPlayerDevice *)ppPlayerList[0])->SetVolume(value);
		if(ppPlayerList)delete []ppPlayerList;//ҵ��ฺ���ͷ�������顣
		return;
	}
	else//���������
	{
		//����Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸��		
		((AVPlayerDevice*)ppPlayerList[0])->SetVolume(value);
		if(ppPlayerList)delete []ppPlayerList;//ҵ��ฺ���ͷ�������顣
	}	
}

uint32 SdkWrapper::GetPlayerVolume()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return MIN_AUDIO_DEVICE_VOLUME;

	AVDevice **ppPlayerList = NULL;
	int playerCount = m_pAudMgr->GetDeviceByType(DEVICE_PLAYER, &ppPlayerList);//��ȡ�������б�
	if(ppPlayerList == NULL)return MIN_AUDIO_DEVICE_VOLUME;

	if(playerCount == 0)
	{
		return MIN_AUDIO_DEVICE_VOLUME;
	}
	else if(playerCount == 1)
	{
		uint32 volume = ((AVPlayerDevice *)ppPlayerList[0])->GetVolume();
		if(ppPlayerList)delete []ppPlayerList;//ҵ��ฺ���ͷ�������顣
		return volume;
	}
	else//���������
	{
		//����Ĭ�ϴ򿪵��ǵ�һ���豸������Ҳ�Ͳ�����һ���豸��		
		uint32 volume = ((AVPlayerDevice*)ppPlayerList[0])->GetVolume();
		if(ppPlayerList)delete []ppPlayerList;//ҵ��ฺ���ͷ�������顣
		return volume;
	}	
}

//����
int SdkWrapper::StartAccompany(std::string playerPath, std::string mediaFilePath, AVAccompanyDevice::SourceType sourceType)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	std::string deviceId = DEVICE_ACCOMPANY;//Ĭ��ֻ��һ�������豸��������������豸��id����ΪDEVICE_ACCOMPANY��
	AVAccompanyDevice *pAccompany = (AVAccompanyDevice *)m_pAudMgr->GetDeviceById(deviceId);
	if (pAccompany == NULL)return AV_ERR_DEVICE_NOT_EXIST;

	pAccompany->SetSource(playerPath, mediaFilePath, sourceType);//���ð���Դ
	return m_pAudMgr->SelectInputDevice(deviceId, true);
}

int SdkWrapper::StopAccompany()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	std::string deviceId = DEVICE_ACCOMPANY;//Ĭ��ֻ��һ�������豸��������������豸��id����ΪDEVICE_ACCOMPANY��
	AVAccompanyDevice *pAccompany = (AVAccompanyDevice *)m_pAudMgr->GetDeviceById(deviceId);
	if (pAccompany == NULL)return AV_ERR_DEVICE_NOT_EXIST;

	return m_pAudMgr->SelectInputDevice(deviceId, false);
}

void SdkWrapper::SetAccompanyVolume(uint32 value)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;

	std::string deviceId = DEVICE_ACCOMPANY;
	AVAccompanyDevice *pAccompany = (AVAccompanyDevice *)m_pAudMgr->GetDeviceById(deviceId);
	if (pAccompany == NULL)return;

	pAccompany->SetVolume(value);
}

uint32 SdkWrapper::GetAccompanyVolume()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return MIN_AUDIO_DEVICE_VOLUME;	

	std::string deviceId = DEVICE_ACCOMPANY;
	AVAccompanyDevice *pAccompany = (AVAccompanyDevice *)m_pAudMgr->GetDeviceById(deviceId);
	if (pAccompany == NULL)return MIN_AUDIO_DEVICE_VOLUME;

	return pAccompany->GetVolume();
}

//�������
int SdkWrapper::StartMicAndAccompanyForMix(std::vector<std::string> localDeviceList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	for (int i = 0; i < localDeviceList.size(); i++)
	{
		std::string deviceId = localDeviceList.at(i);		

		if (deviceId == DEVICE_MIC)
		{
			int retCode = m_pAudMgr->SelectInputDevice(deviceId, true);//����˷�
		}
		else if (deviceId == DEVICE_ACCOMPANY)
		{
			int retCode = StartAccompany("", "", AVAccompanyDevice::AV_ACCOMPANY_SOURCE_TYPE_SYSTEM);//ϵͳ����
		}		
	}

	return AV_OK;
}

int SdkWrapper::StopMicAndAccompanyForMix(std::vector<std::string> localDeviceList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	if(localDeviceList.size() == 0)return AV_ERR_FAILED;

	for (int i = 0; i < localDeviceList.size(); i++)
	{
		std::string deviceId = localDeviceList.at(i);
		
		if (deviceId == DEVICE_MIC)
		{
			int retCode = m_pAudMgr->SelectInputDevice(deviceId, false);
		}
		else if (deviceId == DEVICE_ACCOMPANY)
		{
			int retCode = StopAccompany();
		}		
	}

	return AV_OK;
}

//����ͷ
int SdkWrapper::OpenCamera()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	
	if(m_selectedCameraId == "")return AV_ERR_DEVICE_NOT_EXIST;
  
	AVCameraDevice *pCamera = (AVCameraDevice*)m_pVidMgr->GetDeviceById(m_selectedCameraId);
	if(pCamera == NULL)return AV_ERR_DEVICE_NOT_EXIST;
	//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
	pCamera->SetPreviewParam(m_cameraParam.device_id, m_cameraParam.width, m_cameraParam.height, m_cameraParam.color_format);
	pCamera->SetPreviewCallback(&SdkWrapper::OnLocalVideo, this);//��ȡ�豸����Ƶ��
	pCamera->SetPreTreatmentFun(&SdkWrapper::OnPreTreatmentFun, this);//����Ԥ����ص�
	return m_pVidMgr->SelectInputDevice(m_selectedCameraId, true);

// 	if(m_curSelectedCameraIdx >= 0)//����豸�Ѿ����ڴ�״̬���Ͳ��ٴ������豸
// 	{
// 		return AV_ERR_HAS_IN_THE_STATE;
// 	}
// 
// 	AVDevice **ppCameraList = NULL;
// 	int cameraCount = m_pVidMgr->GetDeviceByType(DEVICE_CAMERA, &ppCameraList);
// 	if (!ppCameraList || cameraCount == 0)return AV_ERR_DEVICE_NOT_EXIST;	
// 
// 	m_curSelectedCameraIdx = 0;
// 	AVCameraDevice *pCamera = (AVCameraDevice*)ppCameraList[m_curSelectedCameraIdx];//Ĭ�ϴ򿪵�һ���豸
// 	//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
// 	pCamera->SetPreviewParam(m_cameraParam.device_id, m_cameraParam.width, m_cameraParam.height, m_cameraParam.color_format);
// 	pCamera->SetPreviewCallback(&SdkWrapper::OnLocalVideo, this);//��ȡ�豸����Ƶ��
// 	int retCode = m_pVidMgr->SelectInputDevice(pCamera->GetId(), true);
// 	delete []ppCameraList;
// 	
// 	return retCode;
}

int SdkWrapper::CloseCamera()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	if(m_selectedCameraId == "")return AV_ERR_DEVICE_NOT_EXIST;
	return m_pVidMgr->SelectInputDevice(m_selectedCameraId, false);

// 	if(m_curSelectedCameraIdx < 0)//����豸�Ѿ����ڹر�״̬���Ͳ��ٹر������豸
// 	{
// 		return AV_ERR_HAS_IN_THE_STATE;
// 	}
// 
// 	AVDevice **ppCameraList = NULL;
// 	int cameraCount = m_pVidMgr->GetDeviceByType(DEVICE_CAMERA, &ppCameraList);
// 	if (!ppCameraList || cameraCount == 0)return AV_ERR_DEVICE_NOT_EXIST;
// 	
// 	AVCameraDevice *pCamera = (AVCameraDevice*)ppCameraList[m_curSelectedCameraIdx];
// 	if (!pCamera)return AV_ERR_DEVICE_NOT_EXIST;
// 	int retCode = m_pVidMgr->SelectInputDevice(pCamera->GetId(), false);
// 	delete []ppCameraList;
// 	m_curSelectedCameraIdx = -1;//��ʾû������ͷ��
// 	return retCode;
}

int SdkWrapper::SetSelectedCameraId(std::string cameraId)
{
	for(int i = 0; i < m_cameraList.size(); i++)
	{
		if(cameraId == m_cameraList[i].first)
		{
			m_selectedCameraId = cameraId;
			return AV_OK;
		}
	}

	return AV_ERR_DEVICE_NOT_EXIST;
}

std::string SdkWrapper::GetSelectedCameraId()
{
	return m_selectedCameraId;
}

int SdkWrapper::GetCameraList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &cameraList)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	AVDevice **ppCameraList = NULL;
	int cameraCount = m_pVidMgr->GetDeviceByType(DEVICE_CAMERA, &ppCameraList);
	if(ppCameraList == NULL || cameraCount == 0)
	{
		return AV_ERR_DEVICE_NOT_EXIST;
	}

    for(int i = 0; i < cameraCount; i++)
    {
        std::pair<std::string/*id*/, std::string/*name*/> camera;
        camera.first = ppCameraList[i]->GetId();
        camera.second = ppCameraList[i]->GetInfo().name;
        cameraList.push_back(camera);
    }
	if(ppCameraList)delete []ppCameraList;//ҵ��ฺ���ͷ�������顣
	return AV_OK;
}

//��Ļ����
int SdkWrapper::OpenScreenShareSend(uint32 left, uint32 top, uint32 right, uint32 bottom, uint32 fps)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	
	AVLocalScreenVideoDevice *pScreenShareSend = (AVLocalScreenVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_LOCAL_SCREEN_VIDEO);
	if(pScreenShareSend == NULL)return AV_ERR_DEVICE_NOT_EXIST;
	//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
	pScreenShareSend->SetPreviewParam(m_screenSendParam.device_id, m_screenSendParam.width, m_screenSendParam.height, m_screenSendParam.color_format);
	pScreenShareSend->SetPreviewCallback(&SdkWrapper::OnLocalVideo, this);//��ȡ�豸����Ƶ��
	//pScreenShareSend->SetPreTreatmentFun(&SdkWrapper::OnPreTreatmentFun, this);//����Ԥ����ص�
	pScreenShareSend->SetScreenCaptureParam(left, top, right, bottom, fps);
	return m_pVidMgr->SelectInputDevice(DEVICE_LOCAL_SCREEN_VIDEO, true);	
}

int SdkWrapper::CloseScreenShareSend()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;

	return m_pVidMgr->SelectInputDevice(DEVICE_LOCAL_SCREEN_VIDEO, false);	
}

void SdkWrapper::GetScreenCaptureParam(uint32 &left, uint32 &top, uint32 &right, uint32 &bottom, uint32 &fps)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return ;

	AVLocalScreenVideoDevice *pScreenShareSend = (AVLocalScreenVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_LOCAL_SCREEN_VIDEO);
	if(pScreenShareSend == NULL)return ;
	
	pScreenShareSend->GetScreenCaptureParam(left, top, right, bottom, fps);
}

void SdkWrapper::EnableScreenShareHDMode(bool isEnable)
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;

	AVLocalScreenVideoDevice *pScreenShareSend = (AVLocalScreenVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_LOCAL_SCREEN_VIDEO);
	if(pScreenShareSend == NULL)return;
	pScreenShareSend->EnableHDMode(isEnable);//������Ļ��Ƶ��ģʽ
}

//�豸����
void SdkWrapper::GetAudioInputDeviceList()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;

	int deviceCount = m_pAudMgr->GetInputDeviceCount();
	for (int i = 0; i < deviceCount; i++)
	{
		AVDevice *pAudioDevice = m_pAudMgr->GetInputDevice(i);
		if (pAudioDevice)
		{
			std::string name = pAudioDevice->GetInfo().name;
			std::string id = pAudioDevice->GetInfo().string_id;
		}			
	}
}

void SdkWrapper::OnRemoteVideo(VideoFrame *pFrameData, void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
	std::vector<std::string> identifierList = pSdkWrapper->GetRequestViewIdentifierList();
	bool bHasRequestView = false;
	for(int i = 0; i < identifierList.size(); i++)
	{
		if (identifierList[i] == pFrameData->identifier)//��Ƶ֡�����ķ����Աid
		{
			bHasRequestView = true;
			break;
		}
	}

	if (!bHasRequestView)
	{
		return;
	}

	DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)pSdkWrapper->m_pMainDlg;
	if(pFrameData->desc.color_format == COLOR_FORMAT_RGB24)//��Ƶ֡ɫ�ʸ�ʽrgb24��ʽ
	{
		if(pFrameData->desc.src_type == VIDEO_SRC_TYPE_CAMERA)//��ƵԴ��������ͷ
		{//���֧���ĸ���Աͬʱֱ����Ƶ
			/*if(pMainDlg->m_viewRemoteVideoSmallRender1.m_identifier == pFrameData->identifier)pMainDlg->m_viewRemoteVideoSmallRender1.DoRender(pFrameData);
			else if(pMainDlg->m_viewRemoteVideoSmallRender2.m_identifier == pFrameData->identifier)pMainDlg->m_viewRemoteVideoSmallRender2.DoRender(pFrameData);
			else if(pMainDlg->m_viewRemoteVideoSmallRender3.m_identifier == pFrameData->identifier)pMainDlg->m_viewRemoteVideoSmallRender3.DoRender(pFrameData);
			else if(pMainDlg->m_viewRemoteVideoSmallRender4.m_identifier == pFrameData->identifier)pMainDlg->m_viewRemoteVideoSmallRender4.DoRender(pFrameData);
		*/}
		else if(pFrameData->desc.src_type == VIDEO_SRC_TYPE_SCREEN)
		{
			if(pMainDlg->m_viewScreenShareRender.m_identifier == pFrameData->identifier)pMainDlg->m_viewScreenShareRender.DoRender(pFrameData);
		}

		if(pMainDlg->m_viewBigVideoRender.m_identifier == pFrameData->identifier
			&& pMainDlg->m_viewBigVideoRender.m_videoSrcType == pFrameData->desc.src_type)
		{
			Dialog1v1VideoRender *p1v1VideoRenderDlg = (Dialog1v1VideoRender*)pSdkWrapper->m_p1v1VideoRenderDlg;
			if(p1v1VideoRenderDlg)
			{
				p1v1VideoRenderDlg->UpdateWindowPos(pFrameData->desc.width, pFrameData->desc.height);
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_identifier = pMainDlg->m_viewBigVideoRender.m_identifier;
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_videoSrcType = pMainDlg->m_viewBigVideoRender.m_videoSrcType;
				p1v1VideoRenderDlg->m_view1v1VideoRender.DoRender(pFrameData);
			}	
			else
			{
				pMainDlg->m_viewBigVideoRender.DoRender(pFrameData);
			}
		}	
	}
	else
	{
		if(pMainDlg->m_viewBigVideoRender.m_identifier == pFrameData->identifier)pMainDlg->SaveRemoteYuvFrame(pFrameData);
	}
}

void SdkWrapper::OnLocalVideo(VideoFrame *pFrameData, void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
	if(pFrameData->desc.src_type == VIDEO_SRC_TYPE_CAMERA && pSdkWrapper->GetDefaultCameraState())//�Լ��д�����ͷ����ȥ��Ⱦ
	{
		DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)pSdkWrapper->m_pMainDlg;

		if(pMainDlg->m_viewLocalVideoRender.m_identifier == pMainDlg->m_config.identifier
			&& pMainDlg->m_viewLocalVideoRender.m_videoSrcType == pFrameData->desc.src_type
			&& pMainDlg->m_isEnableCamera)
		{
			pMainDlg->m_viewLocalVideoRender.DoRender(pFrameData);
		}
		
		if(pMainDlg->m_viewBigVideoRender.m_identifier == pMainDlg->m_config.identifier
			&& pMainDlg->m_viewBigVideoRender.m_videoSrcType == pFrameData->desc.src_type
			&& pMainDlg->m_isEnableCamera)
		{
			Dialog1v1VideoRender *p1v1VideoRenderDlg = (Dialog1v1VideoRender*)pSdkWrapper->m_p1v1VideoRenderDlg;
			if(p1v1VideoRenderDlg)
			{
				p1v1VideoRenderDlg->UpdateWindowPos(pFrameData->desc.width, pFrameData->desc.height);
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_identifier = pMainDlg->m_viewBigVideoRender.m_identifier;
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_videoSrcType = pMainDlg->m_viewBigVideoRender.m_videoSrcType;
				p1v1VideoRenderDlg->m_view1v1VideoRender.DoRender(pFrameData);
			}	
			else
			{
				pMainDlg->m_viewBigVideoRender.DoRender(pFrameData);
			}
		}
	}
	else if(pFrameData->desc.src_type == VIDEO_SRC_TYPE_SCREEN && pSdkWrapper->GetDefaultScreenShareSendState())//�Լ��д���Ļ������ȥ��Ⱦ
	{
		DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)pSdkWrapper->m_pMainDlg;		
		
		if(pMainDlg->m_viewScreenShareRender.m_identifier == pMainDlg->m_config.identifier
			&& pMainDlg->m_viewScreenShareRender.m_videoSrcType == pFrameData->desc.src_type
			&& pMainDlg->m_isEnableScreenShareSend)
		{
			pMainDlg->m_viewScreenShareRender.DoRender(pFrameData);
		}

		if(pMainDlg->m_viewBigVideoRender.m_identifier == pMainDlg->m_config.identifier
			&& pMainDlg->m_viewBigVideoRender.m_videoSrcType == pFrameData->desc.src_type
			&& pMainDlg->m_isEnableScreenShareSend)
		{
			Dialog1v1VideoRender *p1v1VideoRenderDlg = (Dialog1v1VideoRender*)pSdkWrapper->m_p1v1VideoRenderDlg;
			if(p1v1VideoRenderDlg)
			{
				p1v1VideoRenderDlg->UpdateWindowPos(pFrameData->desc.width, pFrameData->desc.height);
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_identifier = pMainDlg->m_viewBigVideoRender.m_identifier;
				p1v1VideoRenderDlg->m_view1v1VideoRender.m_videoSrcType = pMainDlg->m_viewBigVideoRender.m_videoSrcType;
				p1v1VideoRenderDlg->m_view1v1VideoRender.DoRender(pFrameData);
			}	
			else
			{
				pMainDlg->m_viewBigVideoRender.DoRender(pFrameData);
			}
		}
	}
}

std::string SdkWrapper::GetQualityTips()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return "";	
	std::string tipsAll = "";
	std::string tipsAudio = "";
	std::string tipsVideo = "";
	std::string tipsRoom = "";

	tipsAudio = m_pAudCtrl->GetQualityTips();
	tipsVideo = m_pVidCtrl->GetQualityTips();
	tipsRoom = m_pRoom->GetQualityTips();	

	if(tipsRoom != "")
	{
		tipsAll.append(tipsRoom);//׷��
		tipsAll.append("\r\n");
	}	

	if(tipsAudio != "")
	{
		tipsAll.append(tipsAudio);
		tipsAll.append("\r\n");//Windowsϵͳ���棬ÿ�н�β�ǡ�<�س�><����>�������� \r\n��
	}

	if(tipsVideo != "")
	{
		tipsAll.append(tipsVideo);
	}
	
	return tipsAll;
}

std::string SdkWrapper::GetRoomStatParam()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return "";	
	std::string AllParas = "";
    	
	RoomStatParam avParas;
	memset(&avParas, 0, sizeof(RoomStatParam));

	bool result = m_pRoom->GetStatParam(&avParas);
	if (result)
	{

		CString sysparas;
		

			sysparas.Format(_T("ϵͳ��������ʼʱ�䱾��=%u, ����ʱ�䱾��=%u,ϵͳCPUʹ����=%u,����CPUʹ����=%u\r\n"),
				avParas.tick_count_begin,avParas.tick_count_end,avParas.network_param.cpu_rate_sys/100,avParas.network_param.cpu_rate_app/100);
			std::string SysParas=StrWToStrA(sysparas);

			if(SysParas != "")
			{
				AllParas.append(SysParas);
				AllParas.append("\r\n");
			}			
		
		CString netparas;
			netparas.Format(_T("�����������������=%u,��������=%u,���ж�����=%u,����ƽ��������������=%u,�հ�����=%u,�հ�����=%u,���ж�����=%u,����ƽ��������������=%u,����ʱ��=%u\r\n"),
					avParas.network_param.kbps_send,avParas.network_param.packet_send,avParas.network_param.loss_rate_send,avParas.network_param.loss_model_send,
					avParas.network_param.kbps_recv,avParas.network_param.packet_recv,avParas.network_param.loss_rate_recv,avParas.network_param.loss_model_recv,avParas.network_param.rtt);

		std::string NetParas=StrWToStrA(netparas);

		if(NetParas != "")
		{
			AllParas.append(NetParas);
			AllParas.append("\r\n");
		}	
		
		CString ctrlparas;
		ctrlparas.Format(_T("�����·�����Ƶ��������=%d, ͨ����=%d, ��������=%d, ����=%d����Ƶ�����棩���ֱ���=%d * %d������=%d��֡��=%d����Ƶ��С���棩���ֱ���=%d * %d������=%d��֡��=%d\r\n"),avParas.audio_param.qos_param.sample_rate,avParas.audio_param.qos_param.channel_count,avParas.audio_param.qos_param.codec_type,avParas.audio_param.qos_param.bitrate,
			avParas.video_param.qos_param_big.width,avParas.video_param.qos_param_big.height,avParas.video_param.qos_param_big.bitrate,avParas.video_param.qos_param_big.fps,avParas.video_param.qos_param_small.width,avParas.video_param.qos_param_small.height,avParas.video_param.qos_param_small.bitrate,avParas.video_param.qos_param_small.fps);
		
		std::string Ctrlparas=StrWToStrA(ctrlparas);

		if(Ctrlparas != "")
		{
			AllParas.append(Ctrlparas);
			AllParas.append("\r\n");
		}	

		CString videoencparas;
		std::string VideoEncparas;

		int i;

		for (i=0;i<avParas.video_param.encode_count;i++)
		{
			videoencparas.Format(_T("��Ƶ������Ϣ\r\n��������=%u�������=%u�������=%u������ʵʱ֡��=%u���������ʣ��ް�ͷ��=%u\r\n"),avParas.video_param.encode_params[i].view_type,avParas.video_param.encode_params[i].width,avParas.video_param.encode_params[i].height,
				avParas.video_param.encode_params[i].fps,avParas.video_param.encode_params[i].bitrate);
			VideoEncparas=StrWToStrA(videoencparas);
			if(VideoEncparas != "")
			{
				AllParas.append(VideoEncparas);
				AllParas.append("\r\n");
			}	
		}

		CString videodecparas;
		std::string VideoDecparas;
		for (i=0;i<avParas.video_param.decode_count;i++)
		{
			videodecparas.Format(_T("��Ƶ������Ϣ\r\n�û�UIN =%u, ��������=%u�������=%u�������=%u�������֡��=%u����������ʣ��ް�ͷ��=%u\r\n"),avParas.video_param.decode_params[i].tiny_id,avParas.video_param.decode_params[i].view_type,avParas.video_param.decode_params[i].width,
				avParas.video_param.decode_params[i].height,avParas.video_param.decode_params[i].fps,avParas.video_param.decode_params[i].bitrate);
			VideoDecparas=StrWToStrA(videodecparas);
			if(VideoDecparas != "")
			{
				AllParas.append(VideoDecparas);
				AllParas.append("\r\n");
			}	

		}
		
		CString capture;
		capture.Format(_T("�ɼ��ֱ���=%d * %d\r\n\r\n"),avParas.video_param.capture_param.width,avParas.video_param.capture_param.height);
		std::string CapParas=StrWToStrA(capture);

		if(CapParas != "")
		{
			AllParas.append(CapParas);
			AllParas.append("\r\n");
		}	

	}
	else AllParas.append("getparas failed");
	return AllParas;
	
}

void SdkWrapper::OnEnterRoomComplete(int result)
{
	if (result != AV_OK)
	{
		m_pRoom = NULL;
		m_pAudCtrl = NULL;
		m_pVidCtrl = NULL;
		m_pAudMgr = NULL;
		m_pVidMgr = NULL;
		::PostMessage(GetMainHWnd(), WM_ON_ENTER_ROOM, (WPARAM)result, 0);
		return;
	}

	m_pRoom = dynamic_cast<AVRoomMulti*>(m_pContext->GetRoom());
    if (!m_pAudCtrl)
    {
        m_pAudCtrl = dynamic_cast<AVAudioCtrl*>(m_pContext->GetAudioCtrl());
    }

    if (!m_pVidCtrl)
    {
        m_pVidCtrl = dynamic_cast<AVVideoCtrl*>(m_pContext->GetVideoCtrl());
    }   

	m_pAudMgr = dynamic_cast<AVDeviceMgr*>(m_pContext->GetAudioDeviceMgr());
	m_pVidMgr = dynamic_cast<AVDeviceMgr*>(m_pContext->GetVideoDeviceMgr());

	if(!m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)
	{
		//ERROR
		::PostMessage(GetMainHWnd(), WM_ON_ENTER_ROOM, (WPARAM)AV_ERR_FAILED, 0);
		return;
	}

	m_pAudMgr->SetDeviceOperationCallback(OnAudioDeviceOperationCallback, this);
	m_pAudMgr->SetDeviceDetectNotify(OnAudioDeviveDetectNotify,this);
	m_pVidMgr->SetDeviceOperationCallback(OnVideoDeviceOperationCallback, this);

	if (result == AV_OK)
	{	
		//�������������Զ����Ƶ�ص�
		AVRemoteVideoDevice *pVideoDevice = (AVRemoteVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_REMOTE_VIDEO);
		if (pVideoDevice)
		{
			//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
			pVideoDevice->SetPreviewParam(m_remoteVideoParam.device_id, m_remoteVideoParam.width, m_remoteVideoParam.height, m_remoteVideoParam.color_format);
			pVideoDevice->SetPreviewCallback(&SdkWrapper::OnRemoteVideo, this);	
			//m_pVidMgr->SelectOutputDevice(pVideoDevice->GetId().c_str(), true);
		}

		AVRemoteScreenVideoDevice *pScreenShareRecvDevice = (AVRemoteScreenVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_REMOTE_SCREEN_VIDEO);
		if (pScreenShareRecvDevice)
		{
			//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
			pScreenShareRecvDevice->SetPreviewParam(m_screenRecvParam.device_id, m_screenRecvParam.width, m_screenRecvParam.height, m_screenRecvParam.color_format);
			pScreenShareRecvDevice->SetPreviewCallback(&SdkWrapper::OnRemoteVideo, this);	
			//m_pVidMgr->SelectOutputDevice(pScreenShareRecvDevice->GetId().c_str(), true);
		}
	}

  int retCode = AV_OK;
  retCode = GetMicList(m_micList);
  m_selectedMicId = retCode == AV_OK ? m_micList[0].first : "";
 
  retCode =  GetPlayerList(m_playerList);
  m_selectedPlayerId = retCode == AV_OK ? m_playerList[0].first : "";
  
  retCode = GetCameraList(m_cameraList);
  if (m_selectedCameraId == "")
  {
      m_selectedCameraId = retCode == AV_OK ? m_cameraList[0].first : "";
  }
  
	::PostMessage(GetMainHWnd(), WM_ON_ENTER_ROOM, (WPARAM)result, 0);
}

void SdkWrapper::OnExitRoomComplete(int result)
{
	m_pRoom = NULL;
	m_pAudCtrl = NULL;
	m_pVidCtrl = NULL;
	m_pAudMgr = NULL;
	m_pVidMgr = NULL;
	m_micList.clear();
	m_playerList.clear();
	m_cameraList.clear();
  
	m_selectedMicId = "";
	m_selectedPlayerId = "";
	m_selectedCameraId = "";
	m_isSelectedMicEnable = false;
	m_isSelectedPlayerEnable = false;
	m_isSelectedCameraEnable = false;
	m_isSelectedScreenShareSendEnable = false;
	m_curRequestViewIdentifierList.clear();
	m_curRequestViewParamList.clear();
	::PostMessage(GetMainHWnd(), WM_ON_EXIT_ROOM, (WPARAM)result, 0);
}

void SdkWrapper::OnChangeAuthority(int32 ret_code) 
{
  DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)m_pMainDlg;
  pMainDlg->ChangeAuthComplete(ret_code);
}

void SdkWrapper::OnEndpointsUpdateInfo(AVRoom::EndpointEventId eventid, std::vector<std::string> updatelist)
{
	if (eventid == AVRoom::EVENT_ID_ENDPOINT_EXIT)
	{
		for (int i = 0;i < updatelist.size();i++)
		{
			for (int j = 0; j < m_curRequestViewIdentifierList.size(); j++)
			{
				//�����ǰ��Ҫ����Ļ�������Ѿ��뿪���䣬������Ⱦ���Ļ��棬��������档
				if(m_curRequestViewIdentifierList[j] == updatelist[i])
				{
					m_curRequestViewIdentifierList.erase(m_curRequestViewIdentifierList.begin() + j);
					m_curRequestViewParamList.erase(m_curRequestViewParamList.begin() + j);
					DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)m_pMainDlg;
					if(pMainDlg)
					{
						if(pMainDlg->m_viewBigVideoRender.m_identifier == updatelist[i])
						{
							pMainDlg->m_viewBigVideoRender.Clear();
						}

						if(pMainDlg->m_viewLocalVideoRender.m_identifier == updatelist[i])
						{
							pMainDlg->m_viewLocalVideoRender.Clear();
						}

						if(pMainDlg->m_viewScreenShareRender.m_identifier == updatelist[i])
						{
							pMainDlg->m_viewScreenShareRender.Clear();
						}						

						
					}

					Dialog1v1VideoRender *p1v1VideoRenderDlg = (Dialog1v1VideoRender*)m_p1v1VideoRenderDlg;
					if(p1v1VideoRenderDlg && p1v1VideoRenderDlg->m_view1v1VideoRender.m_identifier == updatelist[i])
					{
						p1v1VideoRenderDlg->m_view1v1VideoRender.Clear();
					}
					break;
				}
			}
		}
	}
	else if (eventid == AVRoom::EVENT_ID_ENDPOINT_NO_CAMERA_VIDEO || eventid == AVRoom::EVENT_ID_ENDPOINT_NO_SCREEN_VIDEO)
	{
		VideoSrcType videoSrcType = eventid == AVRoom::EVENT_ID_ENDPOINT_NO_CAMERA_VIDEO ? VIDEO_SRC_TYPE_CAMERA : VIDEO_SRC_TYPE_SCREEN;
		for (int i = 0;i < updatelist.size();i++)
		{
			for (int j = 0; j < m_curRequestViewIdentifierList.size(); j++)
			{
				//�����ǰ��Ҫ����Ļ�������Ѿ��뿪���䣬������Ⱦ���Ļ��棬��������档
				if(m_curRequestViewIdentifierList[j] == updatelist[i] && m_curRequestViewParamList[j].video_src_type == videoSrcType)
				{
					m_curRequestViewIdentifierList.erase(m_curRequestViewIdentifierList.begin() + j);
					m_curRequestViewParamList.erase(m_curRequestViewParamList.begin() + j);
					DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)m_pMainDlg;
					if(pMainDlg)
					{
						if(pMainDlg->m_viewBigVideoRender.m_identifier == updatelist[i] && pMainDlg->m_viewBigVideoRender.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewBigVideoRender.Clear();
						}

						if(pMainDlg->m_viewLocalVideoRender.m_identifier == updatelist[i] && pMainDlg->m_viewLocalVideoRender.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewLocalVideoRender.Clear();
						}

						if(pMainDlg->m_viewScreenShareRender.m_identifier == updatelist[i] && pMainDlg->m_viewScreenShareRender.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewScreenShareRender.Clear();
						}						

						/*if(pMainDlg->m_viewRemoteVideoSmallRender1.m_identifier == updatelist[i] && pMainDlg->m_viewRemoteVideoSmallRender1.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewRemoteVideoSmallRender1.Clear();
						}
						else if(pMainDlg->m_viewRemoteVideoSmallRender2.m_identifier == updatelist[i] && pMainDlg->m_viewRemoteVideoSmallRender2.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewRemoteVideoSmallRender2.Clear();
						}
						else if(pMainDlg->m_viewRemoteVideoSmallRender3.m_identifier == updatelist[i] && pMainDlg->m_viewRemoteVideoSmallRender3.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewRemoteVideoSmallRender3.Clear();
						}
						else if(pMainDlg->m_viewRemoteVideoSmallRender4.m_identifier == updatelist[i] && pMainDlg->m_viewRemoteVideoSmallRender4.m_videoSrcType == videoSrcType)
						{
							pMainDlg->m_viewRemoteVideoSmallRender4.Clear();
						}*/
					}

					Dialog1v1VideoRender *p1v1VideoRenderDlg = (Dialog1v1VideoRender*)m_p1v1VideoRenderDlg;
					if(p1v1VideoRenderDlg && p1v1VideoRenderDlg->m_view1v1VideoRender.m_identifier == updatelist[i] && p1v1VideoRenderDlg->m_view1v1VideoRender.m_videoSrcType == videoSrcType)
					{
						p1v1VideoRenderDlg->m_view1v1VideoRender.Clear();
					}
					break;
				}
			}
		}
	}

	OnEndpointsUpdateInfo();
}

void SdkWrapper::OnEndpointsUpdateInfo()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return;	
	::PostMessage(GetMainHWnd(), WM_ON_ENDPOINTS_UPDATE_INFO, AV_OK, 0);//�������������Ҳ������κ������ռ�
}
void SdkWrapper::OnPrivilegeDiffNotify(int32 privilege)
{
	//TODO, to process room event
}

void SdkWrapper::OnStartContextCompleteCallback(int result, void *pCustomData)
{
	::SendMessage(GetMainHWnd(), WM_ON_START_CONTEXT, (WPARAM)result, 0);
}

void SdkWrapper::OnStopContextCompleteCallbackInternal()
{
	
}

void SdkWrapper::OnStopContextCompleteCallback(void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
	pSdkWrapper->OnStopContextCompleteCallbackInternal();
	
	::PostMessage(GetMainHWnd(), WM_ON_STOP_CONTEXT, (WPARAM)AV_OK, 0);
}

void SdkWrapper::OnRequestViewListCompleteCallback(std::vector<std::string> identifierList, std::vector<View> viewList, int32 result, void *pCustomData)
{
	if(result == AV_OK)
	{			
		SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
		pSdkWrapper->UpdateRequestViewIdentifierList(identifierList);
		pSdkWrapper->UpdateRequestViewParamList(viewList);
	}

	::PostMessage(GetMainHWnd(), WM_ON_REQUEST_VIEW_LIST, (WPARAM)result, 0);	
}

void SdkWrapper::OnCancelAllViewCompleteCallback(int result, void *pCustomData)
{	
	::PostMessage(GetMainHWnd(), WM_ON_CANCEL_ALL_VIEW, (WPARAM)result, 0);
}

void SdkWrapper::OnAudioDeviceOperationCallback(AVDeviceMgr *pAudMgr, AVDevice::DeviceOperation oper, const std::string &deviceId, int retCode, void *pCustomData)
{
	if(!pAudMgr)return;
	AVDevice *pDev = pAudMgr->GetDeviceById(deviceId);
	if(!pDev)return;

	if(pDev->GetType() == DEVICE_MIC)
	{
	    if(retCode == AV_OK)
	    {
			SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
			pSdkWrapper->SetDefaultMicState(oper == AVDevice::DEVICE_OPERATION_OPEN/*���豸*/);
	    }
		::PostMessage(GetMainHWnd(), WM_ON_MIC_OPERATION, (WPARAM)retCode, oper);
	}
	else if(pDev->GetType() == DEVICE_PLAYER)
	{
	    if(retCode == AV_OK)
	    {
	    	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
	      	pSdkWrapper->SetDefaultPlayerState(oper == AVDevice::DEVICE_OPERATION_OPEN);
	    }
		::PostMessage(GetMainHWnd(), WM_ON_PLAYER_OPERATION, (WPARAM)retCode, oper);
	}
	else if(pDev->GetType() == DEVICE_ACCOMPANY)
	{
		//TODO
		::PostMessage(GetMainHWnd(), WM_ON_ACCOMPANY_OPERATION, (WPARAM)retCode, oper);
	}
	else
	{
		//TODO
	}
}

void SdkWrapper::OnVideoDeviceOperationCallback(AVDeviceMgr *pVidMgr, AVDevice::DeviceOperation oper, const std::string &deviceId, int retCode, void *pCustomData)
{
	if(!pVidMgr)return;
	AVDevice *pDev = pVidMgr->GetDeviceById(deviceId);
	if(!pDev)return;

	if(pDev->GetType() == DEVICE_CAMERA)
	{
		if(retCode == AV_OK)
		{
			SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
			pSdkWrapper->SetDefaultCameraState(oper == AVDevice::DEVICE_OPERATION_OPEN);
		}
		::PostMessage(GetMainHWnd(), WM_ON_CAMERA_OPERATION, (WPARAM)retCode, oper);
	}
	else if(pDev->GetType() == DEVICE_REMOTE_VIDEO)
	{
		//TODO
	}	
	else if(pDev->GetType() == DEVICE_LOCAL_SCREEN_VIDEO)
	{
		if(retCode == AV_OK)
		{
			SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
			pSdkWrapper->SetDefaultScreenShareSendState(oper == AVDevice::DEVICE_OPERATION_OPEN);
		}
		::PostMessage(GetMainHWnd(), WM_ON_SCREEN_SHARE_SEND_OPERATION, (WPARAM)retCode, oper);
	}	
	else if(pDev->GetType() == DEVICE_REMOTE_SCREEN_VIDEO)
	{
		//TODO
	}
	else
	{
		//TODO
	}
}

void SdkWrapper::OnPreTreatmentFun(VideoFrame *pFrameData, void *pCustomData)
{
    SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
    if(!pSdkWrapper->GetDefaultCameraState())return;//�Լ�û�д�����ͷ�����˵�
    DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)pSdkWrapper->m_pMainDlg;
    pMainDlg->OnVideoPreTreatment(pFrameData);
}

void SdkWrapper::OnChangeAuthorityCallback(int32 ret_code, void *pCustomData)
{
		SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;
    pSdkWrapper->OnChangeAuthority(ret_code);
}

//�ⲿ����

/*
�����ⲿ�������Ƶ�豸������
ע�⣺
. �ⲿ������ڲ�����ͷ�ǻ���ġ�
. ����������ⲿ�����豸�������󣬾Ͳ�����ʹ���ڲ�����ͷ�ˣ�Ҳ���ǣ����Ҫʹ���ڲ�����ͷ���Ͳ��������ⲿ�����豸��������
. ���Ҫʹ���ⲿ�����豸�������ø�����Ҳ��������ʹ�ã���Ȼ�������ˣ����SDK�ṩ��Ϊ׼ȷ��������Ϣ��
. ���Ҫ���ã������ڽ��뷿��ǰ�����úá�
*/
int SdkWrapper::SetExternalCamAbility(CameraInfo* pCamInfo)//�����ⲿ�������Ƶ�豸����
{
    if (!m_pContext) return AV_ERR_FAILED;

    if (!m_pVidCtrl)
        m_pVidCtrl = dynamic_cast<AVVideoCtrl*>(m_pContext->GetVideoCtrl());  

    if (!m_pVidCtrl) return AV_ERR_FAILED;

    return m_pVidCtrl->SetExternalCapAbility(pCamInfo);
}

int SdkWrapper::OpenExternalCapture()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	AVExternalCapture *pExternalCapture = (AVExternalCapture*)m_pVidMgr->GetDeviceById(DEVICE_EXTERNAL_CAPTURE);
	if(pExternalCapture == NULL)return AV_ERR_DEVICE_NOT_EXIST;
	return m_pVidMgr->SelectInputDevice(DEVICE_EXTERNAL_CAPTURE, true);
}

int SdkWrapper::CloseExternalCapture()
{
	if(!m_pContext || !m_pRoom || !m_pAudCtrl || !m_pVidCtrl || !m_pAudMgr || !m_pVidMgr)return AV_ERR_FAILED;
	return m_pVidMgr->SelectInputDevice(DEVICE_EXTERNAL_CAPTURE, false);
}

int SdkWrapper::FillExternalCaptureFrame(VideoFrame* pFrame)
{
    if (m_pVidMgr)
    {
        AVExternalCapture* pExternalCapture = (AVExternalCapture*)(m_pVidMgr->GetDeviceById(DEVICE_EXTERNAL_CAPTURE));
        return pExternalCapture->OnCaptureFrame(*pFrame);
    }

    return AV_ERR_FAILED;
}

void SdkWrapper::SetVideoParam(AVSupportVideoPreview::PreviewParam param)
{
	m_remoteVideoParam.device_id = "";
	m_remoteVideoParam.width = param.width;
	m_remoteVideoParam.height = param.height;
	m_remoteVideoParam.color_format = param.color_format;
	m_remoteVideoParam.src_type = VIDEO_SRC_TYPE_CAMERA;

	if (m_pVidMgr)
	{
		AVRemoteVideoDevice *pVideoDevice = (AVRemoteVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_REMOTE_VIDEO);
		if (pVideoDevice)
		{
			//��Ƶ��Ⱦǰ�����趨��Ⱦ�����С����ɫ��ʽ����Щ��������ֻӰ�쵽��Ⱦ����Ӱ�쵽����롣
			pVideoDevice->SetPreviewParam(m_remoteVideoParam.device_id, m_remoteVideoParam.width, m_remoteVideoParam.height, m_remoteVideoParam.color_format);
		}
	}
}

void SdkWrapper::ClearVideoParam()
{
  if (m_pVidMgr)
  {
      AVRemoteVideoDevice *pVideoDevice = (AVRemoteVideoDevice*)m_pVidMgr->GetDeviceById(DEVICE_REMOTE_VIDEO);
		  if (pVideoDevice)
		  {
			  //���Ԥ����������ԭʼ���ݡ�
        pVideoDevice->ClearPreviewParam();
		  }
  }
}

void SdkWrapper::EnableHostMode()
{

}
void SdkWrapper::OnAudioDeviveDetectNotify(AVDeviceMgr* device_mgr,DetectedDeviceInfo& info, bool*pbselect, void* custom_data)
{
	if (custom_data){
		SdkWrapper*p = (SdkWrapper*)custom_data;
		p->OnDeviveDetecNotify(device_mgr, info, pbselect, custom_data);
	}
	
}

void SdkWrapper::OnDeviveDetecNotify(AVDeviceMgr* device_mgr,DetectedDeviceInfo& info, bool*pbSelect, void* custom_data)
{
	//�Ƿ������µ��豸
	if (info.isNewDevice)
	{
//		int ret = ::MessageBoxW(GetMainHWnd(), L"�Ƿ�Ҫ�����²�����豸",L"����", MB_OKCANCEL);
// 		if (IDOK != ret && pbSelect ){
// 			*pbSelect = false;
// 		}
// 		else
		{
			//*pbSelect = false;
			if (info.flow == Detect_Mic)
				m_selectedMicId = info.strGuid;
			else
				m_selectedPlayerId = info.strGuid;
		}
	}
	else{
		int deviceCount = device_mgr->GetInputDeviceCount();
		if (info.flow == Detect_Speaker)
			deviceCount = device_mgr->GetOutputDeviceCount();

		if (info.isUsedDevice && deviceCount >0){
// 			int ret = ::MessageBoxW(GetMainHWnd(), L"����ʹ�õ��豸���γ����Ƿ�Ҫ����Ĭ���豸?",L"����", MB_OKCANCEL);
// 			if (IDOK != ret && pbSelect ){
// 				*pbSelect = false;
// 			}
// 			else
			{
				//*pbSelect = false;
				if (info.flow == Detect_Mic)
					m_selectedMicId = device_mgr->GetInputDevice(0)->GetId();
				else
					m_selectedPlayerId = device_mgr->GetOutputDevice(0)->GetId();
			}
		}
		else if (info.isUsedDevice && deviceCount == 0)
		{
			int ret = ::MessageBoxW(GetMainHWnd(), L"��Ƶ�豸������������������豸",L"����", MB_OK);
		}
	}

	//���ж����Ƿ��Ѿ����뷿����
	if (m_pRoom){
		DetectedDeviceInfo *pInfo = new DetectedDeviceInfo();
		*pInfo = info;
		::PostMessage(GetMainHWnd(), WM_ON_AUDIO_DEVICE_DETECT, (WPARAM)(*pbSelect), (LPARAM)pInfo);
	}

}
#ifdef TEST_FOR_INTERNAL
bool SdkWrapper::SetUDTParam(SpeciUDTParam* udt_param)
{
  return false;
}
#endif

int SdkWrapper::StartDeviceTest()
{
	if(m_pContext == NULL)return AV_ERR_FAILED;	
	return m_pContext->StartDeviceTest(SdkWrapper::OnStartDeviceTestCompleteCallback, this);
}

int SdkWrapper::StopDeviceTest()
{
	if(m_pContext == NULL)return AV_ERR_FAILED;	
	m_deviceTestSelectedMicId = "";
	m_deviceTestSelectedPlayerId = "";
	m_deviceTestSelectedCameraId = "";

	m_pDeviceTest = NULL;
	return m_pContext->StopDeviceTest(SdkWrapper::OnStopDeviceTestCompleteCallback, this);//this��һ��ָ�룬��ʱʱ�̿�ָ�����ʵ������
}


void SdkWrapper::DeviceTestSetSelectedMicId(std::string deviceId)
{
	m_deviceTestSelectedMicId = deviceId;
}

int SdkWrapper::DeviceTestOpenMic()
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedMicId, true);
}

int SdkWrapper::DeviceTestCloseMic()
{	
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedMicId, false);
}

int SdkWrapper::DeviceTestGetMicList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceList)
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	int deviceCount = m_pDeviceTest->GetDeviceCountByType(DEVICE_MIC);
	std::vector<std::string> idList = m_pDeviceTest->GetDeviceIdListByType(DEVICE_MIC);
	std::vector<std::string> nameList = m_pDeviceTest->GetDeviceNameListByType(DEVICE_MIC);

	for(int i = 0; i < deviceCount; i++)
	{
		std::pair<std::string/*id*/, std::string/*name*/> device;
		device.first = idList[i];
		device.second = nameList[i];
		deviceList.push_back(device);
	}
	
	return AV_OK;	
}

void SdkWrapper::DeviceTestSetMicVolume(uint32 value)
{
	if (!m_pDeviceTest) return;
	AVMicDevice *pDevice = (AVMicDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedMicId);
	if(pDevice)pDevice->SetVolume(value);
}

uint32 SdkWrapper::DeviceTestGetMicVolume()
{
	if (!m_pDeviceTest) return MIN_AUDIO_DEVICE_VOLUME;
	AVMicDevice *pDevice = (AVMicDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedMicId);
	if(pDevice)return pDevice->GetVolume();
	else return MIN_AUDIO_DEVICE_VOLUME;
}

uint32 SdkWrapper::DeviceTestGetMicDynamicVolume()
{  
	if (!m_pDeviceTest) return MIN_AUDIO_DEVICE_VOLUME;
	AVMicDevice *pDevice = (AVMicDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedMicId);
	if(pDevice)return pDevice->GetDynamicVolume();
	else return MIN_AUDIO_DEVICE_VOLUME;
}


void SdkWrapper::DeviceTestSetSelectedPlayerId(std::string deviceId)
{
	m_deviceTestSelectedPlayerId = deviceId;
}

int SdkWrapper::DeviceTestOpenPlayer()
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedPlayerId, true);
}

int SdkWrapper::DeviceTestClosePlayer()
{  
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedPlayerId, false);
}

int SdkWrapper::DeviceTestGetPlayerList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceList)
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	int deviceCount = m_pDeviceTest->GetDeviceCountByType(DEVICE_PLAYER);
	std::vector<std::string> idList = m_pDeviceTest->GetDeviceIdListByType(DEVICE_PLAYER);
	std::vector<std::string> nameList = m_pDeviceTest->GetDeviceNameListByType(DEVICE_PLAYER);

	for(int i = 0; i < deviceCount; i++)
	{
		std::pair<std::string/*id*/, std::string/*name*/> device;
		device.first = idList[i];
		device.second = nameList[i];
		deviceList.push_back(device);
	}

	return AV_OK;	
}

void SdkWrapper::DeviceTestSetPlayerVolume(uint32 value)
{
	if (!m_pDeviceTest) return;
	AVPlayerDevice *pDevice = (AVPlayerDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedPlayerId);
	if(pDevice)pDevice->SetVolume(value);
}

uint32 SdkWrapper::DeviceTestGetPlayerVolume()
{
	if (!m_pDeviceTest) return MIN_AUDIO_DEVICE_VOLUME;
	AVPlayerDevice *pDevice = (AVPlayerDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedPlayerId);
	if(pDevice)return pDevice->GetVolume();
	else return MIN_AUDIO_DEVICE_VOLUME;
}

uint32 SdkWrapper::DeviceTestGetPlayerDynamicVolume()
{
	if (!m_pDeviceTest) return MIN_AUDIO_DEVICE_VOLUME;
	AVPlayerDevice *pDevice = (AVPlayerDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedPlayerId);
	if(pDevice)return pDevice->GetDynamicVolume();
	else return MIN_AUDIO_DEVICE_VOLUME;
}
	

void SdkWrapper::DeviceTestSetSelectedCameraId(std::string deviceId)
{
	m_deviceTestSelectedCameraId = deviceId;
}

int SdkWrapper::DeviceTestOpenCamera()
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	AVCameraDevice *pDevice = (AVCameraDevice*)m_pDeviceTest->GetDeviceById(m_deviceTestSelectedCameraId);
	if(pDevice)
	{
		pDevice->SetPreviewCallback(&SdkWrapper::OnDeviceTestLocalVideo, this);

		AVSupportVideoPreview::PreviewParam param;
		param.device_id = "";/*��ָ��id������ÿ·��Ƶ�������������*/
		param.width = VIDEO_RENDER_BIG_VIEW_WIDTH;
		param.height = VIDEO_RENDER_BIG_VIEW_HEIGHT;
		param.color_format = COLOR_FORMAT_RGB24;//SDK1.3�汾ֻ֧��COLOR_FORMAT_RGB24��COLOR_FORMAT_I420
		param.src_type = VIDEO_SRC_TYPE_CAMERA;//SDK1.3�汾ֻ֧��VIDEO_SRC_TYPE_CAMERA
		pDevice->SetPreviewParam(param.device_id, param.width, param.height, param.color_format);
	}
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedCameraId, true);
}

int SdkWrapper::DeviceTestCloseCamera()
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	return m_pDeviceTest->EnableDevice(m_deviceTestSelectedCameraId, false);
}

int SdkWrapper::DeviceTestGetCameraList(std::vector<std::pair<std::string/*id*/, std::string/*name*/> > &deviceList)
{
	if (!m_pDeviceTest) return AV_ERR_FAILED;
	int deviceCount = m_pDeviceTest->GetDeviceCountByType(DEVICE_CAMERA);
	std::vector<std::string> idList = m_pDeviceTest->GetDeviceIdListByType(DEVICE_CAMERA);
	std::vector<std::string> nameList = m_pDeviceTest->GetDeviceNameListByType(DEVICE_CAMERA);

	for(int i = 0; i < deviceCount; i++)
	{
		std::pair<std::string/*id*/, std::string/*name*/> device;
		device.first = idList[i];
		device.second = nameList[i];
		deviceList.push_back(device);
	}

	return AV_OK;	
}

void SdkWrapper::OnStartDeviceTestCompleteCallbackInternal(int result)
{
	m_pDeviceTest = dynamic_cast<AVDeviceTest*>(m_pContext->GetDeviceTest());  
	if (!m_pDeviceTest) return;

	m_pDeviceTest->SetDeviceOperationCallback(OnDeviceTestDeviceOperationCallback, this);
	m_deviceTestSelectedMicId = "";
	m_deviceTestSelectedPlayerId = "";
	m_deviceTestSelectedCameraId = "";
}

void SdkWrapper::OnStopDeviceTestCompleteCallbackInternal()
{
	//nothing to do.
}

void SdkWrapper::OnStartDeviceTestCompleteCallback(int result, void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;	
	pSdkWrapper->OnStartDeviceTestCompleteCallbackInternal(result);	
	::PostMessage(GetMainHWnd(), WM_ON_START_DEVICE_TEST, (WPARAM)result, 0);
}

void SdkWrapper::OnStopDeviceTestCompleteCallback(void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;	
	pSdkWrapper->OnStopDeviceTestCompleteCallbackInternal();	
	::PostMessage(GetMainHWnd(), WM_ON_STOP_DEVICE_TEST, (WPARAM)AV_OK, 0);
}
	
void SdkWrapper::OnDeviceTestLocalVideo(VideoFrame *pFrameData, void *pCustomData)
{
	SdkWrapper *pSdkWrapper = (SdkWrapper*)pCustomData;	
	DialogQAVSDKDemo *pMainDlg = (DialogQAVSDKDemo*)pSdkWrapper->m_pMainDlg;
	pMainDlg->m_viewLocalVideoRender.DoRender(pFrameData);
}

void SdkWrapper::OnDeviceTestDeviceOperationCallback(AVDeviceTest *pDeviceTest, AVDevice::DeviceOperation oper, 
	const std::string &deviceId, int retCode, void *pCustomData)
{
	if (!pDeviceTest) return;
	AVDevice *pDev = pDeviceTest->GetDeviceById(deviceId);
	if(!pDev)return;

	if(pDev->GetType() == DEVICE_MIC)
	{
		::PostMessage(GetMainHWnd(), WM_ON_DEVICE_TEST_MIC_OPERATION, (WPARAM)retCode, oper);
	}
	else if(pDev->GetType() == DEVICE_PLAYER)
	{
		::PostMessage(GetMainHWnd(), WM_ON_DEVICE_TEST_PLAYER_OPERATION, (WPARAM)retCode, oper);
	}
	if(pDev->GetType() == DEVICE_CAMERA)
	{
		::PostMessage(GetMainHWnd(), WM_ON_DEVICE_TEST_CAMERA_OPERATION, (WPARAM)retCode, oper);
	}	
	else
	{
		//TODO
	}
}

int SdkWrapper::StartInputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type, std::string path)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	if(path.size() == 0)
		return AV_ERR_FAILED;

	int sr = 0, ch = 0;
	if( 2 > sscanf(path.c_str(), "%d_%d.pcm", &sr, &ch) )
		return AV_ERR_FAILED;

	if(src_type == AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND)
		m_strMixToSendPath = path;
	else if(src_type == AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY)
		m_strMixToPlayPath = path;

	EnableAudioData(src_type, true);
	return m_pAudCtrl->RegistAudioDataCallback(src_type, AudioDataCallback, this);
}

int SdkWrapper::StopInputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	EnableAudioData(src_type, false);
	return m_pAudCtrl->UnregistAudioDataCallback(src_type);
}

int SdkWrapper::StartOutputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	EnableAudioData(src_type, true);
	return m_pAudCtrl->RegistAudioDataCallback(src_type, AudioDataCallback, this);
}

int SdkWrapper::StopOutputAudioDataProcess(AVAudioCtrl::AudioDataSourceType src_type)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	EnableAudioData(src_type, false);
	return m_pAudCtrl->UnregistAudioDataCallback(src_type);
}

int SdkWrapper::EnableAudioData(AVAudioCtrl::AudioDataSourceType src_type, bool enable)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	if(!enable)
	{
		if (src_type == AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM)
		{
			{
				SimpleAutoLock autoLock(&(m_AudioDataLock[src_type]));

				std::map<std::string/*id*/, FILE*>::iterator it = m_mapAudioDataNetStreams.begin();
				while(it != m_mapAudioDataNetStreams.end())
				{
					if( it->second )
					{
						fclose(it->second);
						it->second = NULL;
					}

					it++;
				}
			}

			{
				SimpleAutoLock autoLock(&m_AudioDataMapLock);
				m_mapAudioDataNetStreams.clear();
			}
		}
		else
		{
			std::map<AVAudioCtrl::AudioDataSourceType, FILE*>::iterator it = m_mapAudioData.find(src_type);
			if(it != m_mapAudioData.end())
			{
				SimpleAutoLock autoLock(&(m_AudioDataLock[src_type]));

				if( m_mapAudioData[src_type] )
				{
					fclose(m_mapAudioData[src_type]);

					SimpleAutoLock autoLock(&m_AudioDataMapLock);
					m_mapAudioData.erase(it);
				}
			}
		}
	}

	m_mapAudioDataEnable[src_type] = enable;
	//return m_pAudCtrl->EnableAudioData(src_type, enable);
	return AV_OK;
}

bool SdkWrapper::IsEnableAudioData(AVAudioCtrl::AudioDataSourceType src_type)
{
	if(!m_pAudCtrl)
		return false;

	//return m_pAudCtrl->IsEnableAudioData(src_type);
	return false;
}

int SdkWrapper::SetAudioDataFormat(AVAudioCtrl::AudioDataSourceType src_type, AudioFrameDesc audio_desc)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	return m_pAudCtrl->SetAudioDataFormat(src_type, audio_desc);
}

int SdkWrapper::GetAudioDataFormat(AVAudioCtrl::AudioDataSourceType src_type, AudioFrameDesc& audio_desc)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	return m_pAudCtrl->GetAudioDataFormat(src_type, audio_desc);
}

int SdkWrapper::SetAudioDataVolume(AVAudioCtrl::AudioDataSourceType src_type, float volume)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	return m_pAudCtrl->SetAudioDataVolume(src_type, volume);
}

int SdkWrapper::GetAudioDataVolume(AVAudioCtrl::AudioDataSourceType src_type, float* volume)
{
	if(!m_pAudCtrl)
		return AV_ERR_FAILED;

	return m_pAudCtrl->GetAudioDataVolume(src_type, volume);
}

int SdkWrapper::AudioDataCallback(AudioFrame* audio_frame, AVAudioCtrl::AudioDataSourceType src_type)
{
	if(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND == src_type || (AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY == src_type))
	{
		if(m_mapAudioDataEnable[src_type])
		{
			if(!audio_frame)
				return AV_ERR_FAILED;

			std::string path;
			if(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOSEND == src_type)
				path = m_strMixToSendPath;
			else if(AVAudioCtrl::AUDIO_DATA_SOURCE_MIXTOPLAY == src_type)
				path = m_strMixToPlayPath;

			int sampleRate = 0, channelNum = 0, ret = 0;
			ret = sscanf(path.c_str(), "%d_%d.pcm", &sampleRate, &channelNum);

			if( ret < 2 )
				return AV_ERR_FAILED;

			{
				SimpleAutoLock autoLock(&m_AudioDataMapLock);

				std::map<AVAudioCtrl::AudioDataSourceType, FILE*>::iterator it = m_mapAudioData.find(src_type);
				if(it == m_mapAudioData.end())
					m_mapAudioData.insert(make_pair(src_type, (FILE*)NULL));
			}

			SimpleAutoLock autoLock(&(m_AudioDataLock[src_type]));

			if( !m_mapAudioData[src_type] )
			{
				char szBmpPath[MAX_PATH] = {0};GetModuleFileNameA(NULL, szBmpPath, MAX_PATH);
				PathAppendA(szBmpPath, "..\\"); 
				PathAppendA(szBmpPath, path.c_str());

				m_mapAudioData[src_type] = fopen(szBmpPath, "rb");
			}

			if( m_mapAudioData[src_type] )
			{
				audio_frame->desc.sample_rate = sampleRate;
				audio_frame->desc.channel_num = channelNum;
				audio_frame->data_size = sampleRate * channelNum * 2 / 50;
				audio_frame->desc.bits = 16;

				size_t size = fread(audio_frame->data, sizeof(uint8), audio_frame->data_size, m_mapAudioData[src_type]);
				if( size < audio_frame->data_size || feof(m_mapAudioData[src_type]) ) {
					fseek(m_mapAudioData[src_type], 0, SEEK_SET);
					fread(audio_frame->data, sizeof(uint8), audio_frame->data_size, m_mapAudioData[src_type]);
				}
				return AV_OK;
			}
			else
			{
				return AV_ERR_FAILED;
			}
		}
	}
	else if(AVAudioCtrl::AUDIO_DATA_SOURCE_NETSTREM == src_type)
	{
		if(m_mapAudioDataEnable[src_type])
		{
			if(!audio_frame)
				return AV_ERR_FAILED;

			if(audio_frame->identifier.size() == 0)
				return AV_ERR_FAILED;

			{
				SimpleAutoLock autoLock(&m_AudioDataMapLock);

				std::map<std::string/*id*/, FILE*>::iterator it = m_mapAudioDataNetStreams.find(audio_frame->identifier);
				if(it == m_mapAudioDataNetStreams.end())
					m_mapAudioData.insert(make_pair(src_type, (FILE*)NULL));
			}

			if ( m_AudioDataDesc[src_type].channel_num != audio_frame->desc.channel_num ||
					 m_AudioDataDesc[src_type].sample_rate != audio_frame->desc.sample_rate )
			{
				m_AudioDataDesc[src_type].channel_num = audio_frame->desc.channel_num;
				m_AudioDataDesc[src_type].sample_rate = audio_frame->desc.sample_rate;

				SimpleAutoLock autoLock(&m_AudioDataMapLock);

				std::map<std::string/*id*/, FILE*>::iterator it = m_mapAudioDataNetStreams.begin();
				while(it != m_mapAudioDataNetStreams.end())
				{
					if( it->second )
					{
						fclose(it->second);
						it->second = NULL;
					}

					it++;
				}
			}

			SimpleAutoLock autoLock(&(m_AudioDataLock[src_type]));

			if( !m_mapAudioDataNetStreams[audio_frame->identifier] )
			{
				SYSTEMTIME time = {0}; ::GetLocalTime(&time); FILETIME ftime = {0}; ::SystemTimeToFileTime(&time, &ftime);

				char strExt[256] = {0};
				sprintf(strExt, "%s_%s_%d_%d_%u", m_mapAudioDataName[src_type].c_str(), audio_frame->identifier.c_str(),
					audio_frame->desc.sample_rate, audio_frame->desc.channel_num, ftime.dwLowDateTime);

				std::string remoteViewFileName = std::string(strExt) + ".pcm";

				char szBmpPath[MAX_PATH] = {0};GetModuleFileNameA(NULL, szBmpPath, MAX_PATH);
				PathAppendA(szBmpPath, "..\\"); 
				PathAppendA(szBmpPath, remoteViewFileName.c_str());

				m_mapAudioDataNetStreams[audio_frame->identifier] = fopen(szBmpPath, "wb+");
			}

			if( m_mapAudioDataNetStreams[audio_frame->identifier] )
			{
				fwrite(audio_frame->data, sizeof(uint8), audio_frame->data_size, m_mapAudioDataNetStreams[audio_frame->identifier]);
				fflush(m_mapAudioDataNetStreams[audio_frame->identifier]);
				return AV_OK;
			}
			else
			{
				return AV_ERR_FAILED;
			}
		}
	}
	else
	{
		if(m_mapAudioDataEnable[src_type])
		{
			if(!audio_frame)
				return AV_ERR_FAILED;

			{
				SimpleAutoLock autoLock(&m_AudioDataMapLock);

				std::map<AVAudioCtrl::AudioDataSourceType, FILE*>::iterator it = m_mapAudioData.find(src_type);
				if(it == m_mapAudioData.end())
					m_mapAudioData.insert(make_pair(src_type, (FILE*)NULL));
			}

			if ( m_AudioDataDesc[src_type].channel_num != audio_frame->desc.channel_num ||
					 m_AudioDataDesc[src_type].sample_rate != audio_frame->desc.sample_rate )
			{
				m_AudioDataDesc[src_type].channel_num = audio_frame->desc.channel_num;
				m_AudioDataDesc[src_type].sample_rate = audio_frame->desc.sample_rate;

				if( m_mapAudioData[src_type] )
				{
					{
						SimpleAutoLock autoLock(&(m_AudioDataLock[src_type]));
						fclose(m_mapAudioData[src_type]);
					}

					{
						SimpleAutoLock autoLock(&m_AudioDataMapLock);
						m_mapAudioData[src_type] = NULL;
					}
				}
			}

			SimpleAutoLock autoLock(&(m_AudioDataLock[src_type])); 

			if( !m_mapAudioData[src_type] )
			{
				SYSTEMTIME time = {0}; ::GetLocalTime(&time); FILETIME ftime = {0}; ::SystemTimeToFileTime(&time, &ftime);

				char strExt[256] = {0};
				sprintf(strExt, "%s_%d_%d_%u", m_mapAudioDataName[src_type].c_str(), 
					audio_frame->desc.sample_rate, audio_frame->desc.channel_num, ftime.dwLowDateTime);

				std::string remoteViewFileName = std::string(strExt) + ".pcm";

				char szBmpPath[MAX_PATH] = {0};GetModuleFileNameA(NULL, szBmpPath, MAX_PATH);
				PathAppendA(szBmpPath, "..\\"); 
				PathAppendA(szBmpPath, remoteViewFileName.c_str());

				m_mapAudioData[src_type] = fopen(szBmpPath, "wb+");
			}

			if( m_mapAudioData[src_type] )
			{
				fwrite(audio_frame->data, sizeof(uint8), audio_frame->data_size, m_mapAudioData[src_type]);
				fflush(m_mapAudioData[src_type]);
				return AV_OK;
			}
			else
			{
				return AV_ERR_FAILED;
			}
		}
	}
}

int SdkWrapper::AudioDataCallback(AudioFrame* audio_frame, AVAudioCtrl::AudioDataSourceType src_type, void* custom_data)
{
	return ((SdkWrapper*)custom_data)->AudioDataCallback(audio_frame, src_type);
}

void SdkWrapper::Set1v1VideoRenderDlg(void *p1v1VideoRenderDlg)
{
	m_p1v1VideoRenderDlg = p1v1VideoRenderDlg;
}

const AVRoom::Info* SdkWrapper::GetRoomInfo()
{
	return m_pRoom->GetRoomInfo();
}

