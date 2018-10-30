#include "StdAfx.h"
#include "ConfigInfoMgr.h"
#include "Util.h"

#define MAX_FIELD_LEN 100
#define MAX_USER_SIG_LEN 1024
#define MAX_CHANNEL_DESCRIPTION_LEN 1024  //自己加的

ConfigInfoMgr* ConfigInfoMgr::GetInst()
{
	static ConfigInfoMgr info;
	return &info;
}

ConfigInfoMgr::ConfigInfoMgr()
{
	m_configInfoFileName = GetExePath() + _T("ConfigInfo.ini");
	m_appNameConfigInfo = _T("ConfigInfo");
	m_keyNameAppCount = _T("AppCount");
	m_keyNameSceneType = _T("SceneType");
	m_keyNameAccountType = _T("AccountType");
	m_keyNameAppIdAt3rd = _T("AppIdAt3rd");
	m_keyNameSdkAppId = _T("SdkAppId");

	m_keyNameRoomId = _T("RoomId");
	m_keyNameAccountCount = _T("AccountCount");
	m_keyNameIdentifier = _T("Identifier");
	m_keyNameUserSig = _T("UserSig");
	
	//这三行自己加的
	m_keyNameChannelCount = _T("ChannelCount");;
	m_keyNameChannelName = _T("ChannelName");;
	m_keyNameChannelDescription = _T("ChannelDescription");

	m_configInfo.roomId = _T("");

}

ConfigInfoMgr::~ConfigInfoMgr(void)
{
			
}


void ConfigInfoMgr::LoadConfigInfo()
{
	m_configInfo.appInfoList.clear();
	m_configInfo.accountInfoList.clear();
	m_configInfo.channelInfoList.clear();  //自己加的

	CFileFind fFind;
	bool isExist = fFind.FindFile(m_configInfoFileName); 
	if(!isExist)
	{		
		//创建配置文件
		string fileName = StrWToStrA(m_configInfoFileName);
		FILE *pFile;
		pFile = fopen(fileName.c_str(),"w");
		fclose(pFile);
	}
	else
	{
		int appCount = ::GetPrivateProfileIntW(m_appNameConfigInfo, m_keyNameAppCount, 0, m_configInfoFileName);

		for(int i = 0; i < appCount; i++)
		{		
			CString keyNameDemoTypeTmp;
			CString keyNameAccountTypeTmp;
			CString keyNameAppIdAt3rdTmp;
			CString keyNameSdkAppIdTmp;

			keyNameDemoTypeTmp.Format(_T("%s%003d"), m_keyNameSceneType.GetString(), i);
			keyNameAccountTypeTmp.Format(_T("%s%003d"), m_keyNameAccountType.GetString(), i);
			keyNameAppIdAt3rdTmp.Format(_T("%s%003d"), m_keyNameAppIdAt3rd.GetString(), i);
			keyNameSdkAppIdTmp.Format(_T("%s%003d"), m_keyNameSdkAppId.GetString(), i);

			TCHAR demoTypeTmp[MAX_FIELD_LEN];
			TCHAR uidTypeTmp[MAX_FIELD_LEN];
			TCHAR openAppIdTmp[MAX_FIELD_LEN];
			TCHAR sdkAppIdTmp[MAX_FIELD_LEN];

			if (::GetPrivateProfileString(m_appNameConfigInfo, keyNameDemoTypeTmp, _T(""), demoTypeTmp, MAX_FIELD_LEN, m_configInfoFileName)
				&& ::GetPrivateProfileString(m_appNameConfigInfo, keyNameAccountTypeTmp, _T(""), uidTypeTmp, MAX_FIELD_LEN, m_configInfoFileName)
				&& ::GetPrivateProfileString(m_appNameConfigInfo, keyNameAppIdAt3rdTmp, _T(""), openAppIdTmp, MAX_FIELD_LEN, m_configInfoFileName)
				&& ::GetPrivateProfileString(m_appNameConfigInfo, keyNameSdkAppIdTmp, _T(""), sdkAppIdTmp, MAX_FIELD_LEN, m_configInfoFileName)
				)
			{
				AppInfo info;
				info.sceneType = demoTypeTmp;
				info.accountType = uidTypeTmp;
				info.appIdAt3rd = openAppIdTmp;
				info.sdkAppId = sdkAppIdTmp;
				m_configInfo.appInfoList.push_back(info);
			}
		}

		TCHAR roomIdTmp[MAX_FIELD_LEN] = {0};
		GetPrivateProfileString(m_appNameConfigInfo, m_keyNameRoomId, _T(""), roomIdTmp, MAX_FIELD_LEN, m_configInfoFileName);
		m_configInfo.roomId = roomIdTmp;

		int accountCount = ::GetPrivateProfileIntW(m_appNameConfigInfo, m_keyNameAccountCount, 0, m_configInfoFileName);

		for(int i = 0; i < accountCount; i++)
		{		
			CString keyNameIdentifierTmp;
			CString keyNameUserSigTmp;

			keyNameIdentifierTmp.Format(_T("%s%003d"), m_keyNameIdentifier.GetString(), i);
			keyNameUserSigTmp.Format(_T("%s%003d"), m_keyNameUserSig.GetString(), i);

			AccountInfo info;
			TCHAR identifierTmp[MAX_FIELD_LEN];
			TCHAR userSigTmp[MAX_USER_SIG_LEN];

			if (::GetPrivateProfileString(m_appNameConfigInfo, keyNameIdentifierTmp, _T(""), identifierTmp, MAX_FIELD_LEN, m_configInfoFileName)
				&& ::GetPrivateProfileString(m_appNameConfigInfo, keyNameUserSigTmp, _T(""), userSigTmp, MAX_USER_SIG_LEN, m_configInfoFileName)
				)
			{
				info.identifier = identifierTmp;
				info.userSig = userSigTmp;
				m_configInfo.accountInfoList.push_back(info);
			}
		}

		//到下一个注释都是自己加的
		int channelCount = ::GetPrivateProfileIntW(m_appNameConfigInfo, m_keyNameChannelCount, 0, m_configInfoFileName);

		for(int i = 0; i < channelCount; i++)
		{		
			CString keyNameChannelNameTmp;
			CString keyNameChannelDescriptionTmp;

			keyNameChannelNameTmp.Format(_T("%s%003d"), m_keyNameChannelName.GetString(), i);
			keyNameChannelDescriptionTmp.Format(_T("%s%003d"), m_keyNameChannelDescription.GetString(), i);

			ChannelInfo info;
			TCHAR channelNameTmp[MAX_FIELD_LEN];
			TCHAR channelDescriptionTmp[MAX_CHANNEL_DESCRIPTION_LEN];

			if (::GetPrivateProfileString(m_appNameConfigInfo, keyNameChannelNameTmp, _T(""), channelNameTmp, MAX_FIELD_LEN, m_configInfoFileName)
				&& ::GetPrivateProfileString(m_appNameConfigInfo, keyNameChannelDescriptionTmp, _T(""), channelDescriptionTmp, MAX_CHANNEL_DESCRIPTION_LEN, m_configInfoFileName)
				)
			{
				info.channelName = channelNameTmp;
				info.channelDescription = channelDescriptionTmp;
				m_configInfo.channelInfoList.push_back(info);
			}
		}  //自己加的
	}

	if(m_configInfo.appInfoList.size() == 0)
	{
		//默认配置
		AppInfo info;
		info.sceneType = _T("实时通信场景体验版");
		info.accountType = _T("107");
		info.appIdAt3rd = _T("1104620500");
		info.sdkAppId = _T("1104620500");
		m_configInfo.appInfoList.push_back(info);

		info.sceneType = _T("主播场景体验版");
		info.accountType = _T("107");
		info.appIdAt3rd = _T("1104062745");
		info.sdkAppId = _T("1104062745");
		m_configInfo.appInfoList.push_back(info);
	}
}

void ConfigInfoMgr::SaveConfigInfo()
{
	::DeleteFile(m_configInfoFileName);

	CString appCountTmp;
	appCountTmp.Format(_T("%u"), m_configInfo.appInfoList.size());
	::WritePrivateProfileString(m_appNameConfigInfo, m_keyNameAppCount, appCountTmp, m_configInfoFileName);

	for(int i = 0; i < m_configInfo.appInfoList.size(); i++)
	{		
		CString keyNameSceneTypeTmp;
		CString keyNameAccountTypeTmp;
		CString keyNameAppIdAt3rdTmp;
		CString keyNameSdkAppIdTmp;
		keyNameSceneTypeTmp.Format(_T("%s%003d"), m_keyNameSceneType.GetString(), i);
		keyNameAccountTypeTmp.Format(_T("%s%003d"), m_keyNameAccountType.GetString(), i);
		keyNameAppIdAt3rdTmp.Format(_T("%s%003d"), m_keyNameAppIdAt3rd.GetString(), i);
		keyNameSdkAppIdTmp.Format(_T("%s%003d"), m_keyNameSdkAppId.GetString(), i);

		::WritePrivateProfileString(m_appNameConfigInfo, keyNameSceneTypeTmp, m_configInfo.appInfoList[i].sceneType, m_configInfoFileName);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameAccountTypeTmp, m_configInfo.appInfoList[i].accountType, m_configInfoFileName);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameAppIdAt3rdTmp, m_configInfo.appInfoList[i].appIdAt3rd, m_configInfoFileName);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameSdkAppIdTmp, m_configInfo.appInfoList[i].sdkAppId, m_configInfoFileName);
	}

	::WritePrivateProfileString(m_appNameConfigInfo, m_keyNameRoomId, m_configInfo.roomId, m_configInfoFileName);

	CString accountCountTmp;
	accountCountTmp.Format(_T("%u"), m_configInfo.accountInfoList.size());
	::WritePrivateProfileString(m_appNameConfigInfo, m_keyNameAccountCount, accountCountTmp, m_configInfoFileName);

	for(int i = 0; i < m_configInfo.accountInfoList.size(); i++)
	{		
		CString keyNameIdentifierTmp;
		CString keyNameUserSigTmp;
		keyNameIdentifierTmp.Format(_T("%s%003d"), m_keyNameIdentifier.GetString(), i);
		keyNameUserSigTmp.Format(_T("%s%003d"), m_keyNameUserSig.GetString(), i);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameIdentifierTmp, m_configInfo.accountInfoList[i].identifier, m_configInfoFileName);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameUserSigTmp, m_configInfo.accountInfoList[i].userSig, m_configInfoFileName);
	}

	//后面是自己加的
	CString channelCountTmp;
	channelCountTmp.Format(_T("%u"), m_configInfo.channelInfoList.size());
	::WritePrivateProfileString(m_appNameConfigInfo, m_keyNameChannelCount, channelCountTmp, m_configInfoFileName);

	for(int i = 0; i < m_configInfo.channelInfoList.size(); i++)
	{		
		CString keyNameChannelNameTmp;
		CString keyNameChannelDescriptionTmp;
		keyNameChannelNameTmp.Format(_T("%s%003d"), m_keyNameChannelName.GetString(), i);
		keyNameChannelDescriptionTmp.Format(_T("%s%003d"), m_keyNameChannelDescription.GetString(), i);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameChannelNameTmp, m_configInfo.channelInfoList[i].channelName, m_configInfoFileName);
		::WritePrivateProfileString(m_appNameConfigInfo, keyNameChannelDescriptionTmp, m_configInfo.channelInfoList[i].channelDescription, m_configInfoFileName);
	}
}


void ConfigInfoMgr::AddApp(AppInfo info)
{
	for(int i = 0; i < m_configInfo.appInfoList.size(); i++)
	{
		if (m_configInfo.appInfoList[i].sceneType == info.sceneType)
		{
			m_configInfo.appInfoList[i] = info;
			return;
		}
	}
	m_configInfo.appInfoList.push_back(info);
}

void ConfigInfoMgr::RemoveApp(CString sceneType)
{
	for(int i = 0; i < m_configInfo.appInfoList.size(); i++)
	{
		if (m_configInfo.appInfoList[i].sceneType == sceneType)
		{
			m_configInfo.appInfoList.erase(m_configInfo.appInfoList.begin() + i);
			return;
		}
	}	
}

AppInfolist ConfigInfoMgr::GetAppInfoList()
{
	return m_configInfo.appInfoList;
}

AppInfo ConfigInfoMgr::GetAppInfoByIndex(int index)
{
	if (m_configInfo.appInfoList.size() > 0 && index >= 0 && index < m_configInfo.appInfoList.size())
	{
		return m_configInfo.appInfoList[index];
	}

	AppInfo info;
	info.sceneType = _T("");
	info.accountType = _T("");
	info.appIdAt3rd = _T("");
	info.sdkAppId = _T("");
	return info;
}

AppInfo ConfigInfoMgr::GetAppInfoById(CString sceneType)
{
	if(m_configInfo.appInfoList.size() > 0)
	{
		for(int i = 0; i < m_configInfo.appInfoList.size(); i++)
		{
			if (m_configInfo.appInfoList[i].sceneType == sceneType)
			{
				return m_configInfo.appInfoList[i];
			}
		}
	}
	else
	{
		AppInfo info;
		info.sceneType = _T("");
		info.accountType = _T("");
		info.appIdAt3rd = _T("");
		info.sdkAppId = _T("");
		return info;
	}
}

void ConfigInfoMgr::AddAccount(AccountInfo info)
{
	for(int i = 0; i < m_configInfo.accountInfoList.size(); i++)
	{
		if (m_configInfo.accountInfoList[i].identifier == info.identifier)
		{
			m_configInfo.accountInfoList[i] = info;
			return;
		}
	}
	
	m_configInfo.accountInfoList.push_back(info);
}

void ConfigInfoMgr::RemoveAccount(CString identifier)
{
	for(int i = 0; i < m_configInfo.accountInfoList.size(); i++)
	{
		if (m_configInfo.accountInfoList[i].identifier == identifier)
		{
			m_configInfo.accountInfoList.erase(m_configInfo.accountInfoList.begin() + i);
			return;
		}
	}	
}

AccountInfolist ConfigInfoMgr::GetAccountInfoList()
{
	return m_configInfo.accountInfoList;
}

AccountInfo ConfigInfoMgr::GetAccountInfoByIndex(int index)
{
	if (m_configInfo.accountInfoList.size() > 0 && index >= 0 && index < m_configInfo.accountInfoList.size())
	{
		return m_configInfo.accountInfoList[index];
	}

	AccountInfo accountInfo;
	accountInfo.identifier = _T("");
	accountInfo.userSig = _T("");
	return accountInfo;
}

AccountInfo ConfigInfoMgr::GetAccountInfoById(CString identifier)
{
	if(m_configInfo.accountInfoList.size() > 0)
	{
		for(int i = 0; i < m_configInfo.accountInfoList.size(); i++)
		{
			if (m_configInfo.accountInfoList[i].identifier == identifier)
			{
				return m_configInfo.accountInfoList[i];
			}
		}
	}
	else
	{
		AccountInfo info;
		info.identifier = _T("");
		info.userSig = _T("");
		return info;
	}
}

void ConfigInfoMgr::SetRoomId(CString roomId)
{
	m_configInfo.roomId = roomId;
}

CString ConfigInfoMgr::GetRoomId()
{
	return m_configInfo.roomId;
}


//后面都是自己加的
void ConfigInfoMgr::AddChannel(ChannelInfo info)
{
	for(int i = 0; i < m_configInfo.channelInfoList.size(); i++)
	{
		if (m_configInfo.channelInfoList[i].channelName == info.channelName)
		{
			m_configInfo.channelInfoList[i] = info;
			return;
		}
	}
	
	m_configInfo.channelInfoList.push_back(info);
}

void ConfigInfoMgr::RemoveChannel(CString channelName)
{
	for(int i = 0; i < m_configInfo.channelInfoList.size(); i++)
	{
		if (m_configInfo.channelInfoList[i].channelName == channelName)
		{
			m_configInfo.channelInfoList.erase(m_configInfo.channelInfoList.begin() + i);
			return;
		}
	}	
}

ChannelInfolist ConfigInfoMgr::GetChannelInfoList()
{
	return m_configInfo.channelInfoList;
}

ChannelInfo ConfigInfoMgr::GetChannelInfoByIndex(int index)
{
	if (m_configInfo.channelInfoList.size() > 0 && index >= 0 && index < m_configInfo.channelInfoList.size())
	{
		return m_configInfo.channelInfoList[index];
	}

	ChannelInfo channelInfo;
	channelInfo.channelName = _T("");
	channelInfo.channelDescription = _T("");
	return channelInfo;
}

ChannelInfo ConfigInfoMgr::GetChannelInfoById(CString channelName)
{
	if(m_configInfo.channelInfoList.size() > 0)
	{
		for(int i = 0; i < m_configInfo.channelInfoList.size(); i++)
		{
			if (m_configInfo.channelInfoList[i].channelName == channelName)
			{
				return m_configInfo.channelInfoList[i];
			}
		}
	}
	else
	{
		ChannelInfo info;
		info.channelName = _T("");
		info.channelDescription = _T("");
		return info;
	}
}