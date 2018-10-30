#ifndef _TIM_COMM_C_H_
#define _TIM_COMM_C_H_

#if defined(_MSC_VER) && (_MSC_VER < 1600)
	typedef signed char       int8_t;
	typedef signed short      int16_t;
	typedef signed int        int32_t;
	typedef unsigned char     uint8_t;
	typedef unsigned short    uint16_t;
	typedef unsigned int      uint32_t;
	typedef signed __int64       int64_t;
	typedef unsigned __int64     uint64_t;
#else
	#include <stdint.h>
#endif


#ifdef _WIN32
#define TIM_DECL __declspec(dllexport) 
#else 
#define TIM_DECL 
#endif



extern "C" 
{
	/**
	* ��������״̬
	*/
	enum TIMNetworkStatus 
	{
		TIM_NETWORK_STATUS_CONNECTED = 1, //������
		TIM_NETWORK_STATUS_DISCONNECTED = 2, //���ӶϿ�
	};

	enum TIMConversationType {
		kCnvInvalid,
		kCnvC2C,
		kCnvGroup,
		kCnvSysem,
	};

	enum TIMElemType {
		kElemInvalid,
		kElemText,
		kElemImage,
		kElemSound,
		kElemFile,
		kElemCustom,
		kElemFace,
		kElemLocation,
		kElemGroupTips,
		kElemGroupReport,
		kElemProfileChange,
		kElemFriendChange,
		kElemVideo,
	};

	typedef enum _TIMImageType
	{
		kOriginalImage = 0,
		kThumbImage,
		kLargeImage,
	}TIMImageType;

	typedef enum _E_TIM_IMAGE_COMPRESS_TYPE
	{
		TIM_IMAGE_COMPRESS_ORIGIN = 0x00, //ԭͼ(��ѹ����
		TIM_IMAGE_COMPRESS_HIGH = 0x01, //��ѹ���ʣ�ͼƬ��С��Ĭ��ֵ
		TIM_IMAGE_COMPRESS_LOW = 0x02,//��ѹ��������ͼ����(ͼƬ�ϴ�)
	}TIM_IMAGE_COMPRESS_TYPE;

	enum TIMLogLevel
	{
		klogNone = 0,
		kLogError,
		kLogWarn,
		kLogInfo,
		kLogDebug
	};

	typedef enum _E_TIMGroupPendencyHandleResult
	{
		TIMGroupPendencyRejected = 0,
		TIMGroupPendencyConfirmed = 1,
	}TIMGroupPendencyHandleResult;

	typedef struct _TIMUser_c
	{
		char* account_type;
		char* app_id_at_3rd;
		char* identifier;
		char* stiny_id;
		uint64_t tiny_id;
	}TIMUserInfo;

	typedef void (*CBOnSuccess) (void* data);
	typedef void (*CBOnError) (int code, const char* desc, void* data);

	typedef struct _TIMCallBack_c
	{
		CBOnSuccess OnSuccess;
		CBOnError	OnError;
		void*		data;
	}TIMCommCB;

	typedef void (*CBRequestOnSuccess) (const char* buf, uint32_t len, void* data);
	typedef void (*CBRequestOnError) (int code, const char* desc, void* data);

	typedef struct _TIMRequestCallBack_c
	{
		CBRequestOnSuccess  OnSuccess;
		CBRequestOnError	OnError;
		void*		data;
	}TIMRequestCB;

	typedef void (*CBConnOnConnected) (void* data);
	typedef void (*CBConnOnDisconnected) (void* data);

	typedef struct _TIMConnCallBack_c
	{
		CBConnOnConnected		OnConnected;
		CBConnOnDisconnected	OnDisconnected;
		void* data;
	}TIMConnCB;

	typedef void (*CBKickOffline) (void* data);
	typedef struct _TIMKickOfflineCallBack_c
	{
		CBKickOffline OnKickOffline;
		void* data;
	}TIMForceOfflineCB;

	typedef void* TIMConversationHandle;

	typedef void* TIMProfileHandle;
	typedef void* TIMGroupMemberInfoHandle;
};

#endif