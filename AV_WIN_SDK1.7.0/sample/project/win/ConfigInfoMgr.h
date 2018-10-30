#pragma once
#include <vector>
using namespace std;

typedef struct tagAppInfo
{
	CString sceneType;
	CString accountType;
	CString appIdAt3rd;
	CString sdkAppId;
}AppInfo;

typedef struct tagAccountInfo
{
	CString identifier;
	CString userSig;
}AccountInfo;

typedef struct tagChannelInfo  //自己加的
{
	CString channelName;
	CString channelDescription;
}ChannelInfo;

typedef vector<AppInfo>AppInfolist;
typedef vector<AccountInfo>AccountInfolist;
typedef vector<ChannelInfo>ChannelInfolist;  //自己加的

typedef struct tagConfigInfo
{
	AppInfolist appInfoList;
	CString roomId;
	AccountInfolist accountInfoList;
	ChannelInfolist channelInfoList;  //自己加的
}ConfigInfo;

class ConfigInfoMgr
{
private:
	ConfigInfoMgr();
public:
	~ConfigInfoMgr(void);
	static ConfigInfoMgr *GetInst();
public:
	void LoadConfigInfo();
	void SaveConfigInfo();

	void AddApp(AppInfo info);
	void RemoveApp(CString sceneType);
	AppInfolist	GetAppInfoList();
	AppInfo	GetAppInfoByIndex(int index);
	AppInfo	GetAppInfoById(CString sceneType);	
	
	void SetRoomId(CString roomId);
	CString	GetRoomId();

	void AddAccount(AccountInfo info);
	void RemoveAccount(CString identifier);
	AccountInfolist	GetAccountInfoList();
	AccountInfo	GetAccountInfoByIndex(int index);
	AccountInfo	GetAccountInfoById(CString identifier);	
	
	//这五行是自己加的
	void AddChannel(ChannelInfo info);
	void RemoveChannel(CString identifier);
	ChannelInfolist	GetChannelInfoList();
	ChannelInfo	GetChannelInfoByIndex(int index);
	ChannelInfo	GetChannelInfoById(CString channelName);	

private:
	CString m_configInfoFileName;
	CString m_appNameConfigInfo;
	CString m_keyNameAppCount;
	CString m_keyNameSceneType;
	CString m_keyNameAccountType;
	CString m_keyNameAppIdAt3rd;
	CString m_keyNameSdkAppId;
	
	CString m_keyNameRoomId;
	CString m_keyNameAccountCount;
	CString m_keyNameIdentifier;
	CString m_keyNameUserSig;
    
	//这三行是自己加的
	CString m_keyNameChannelCount;
	CString m_keyNameChannelName;
	CString m_keyNameChannelDescription;

	ConfigInfo m_configInfo;
};

