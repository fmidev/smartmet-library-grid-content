#pragma once

#include "UserInfo.h"
#include "ServiceResults.h"
#include <grid-files/common/Log.h>

#include <vector>


namespace SmartMet
{
namespace UserManagement
{




class ServiceInterface
{
  public:
                    ServiceInterface();
     virtual        ~ServiceInterface();

     virtual Log*   getDebugLog();
     virtual Log*   getProcessingLog();
     virtual void   setDebugLog(Log *debugLog);
     virtual void   setProcessingLog(Log *processingLog);

     virtual void   setEnabled(bool enabled);
     virtual bool   isEnabled();
     virtual void   shutdown();

     virtual int    addUser(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    deleteUser(T::SessionId sessionId,uint userId);
     virtual int    updateUserInfo(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue);
     virtual int    getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo);
     virtual int    getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo);
     virtual int    setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue);
     virtual int    setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName);
     virtual int    deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup);
     virtual int    deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList);

  protected:

     virtual int    _addUser(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    _deleteUser(T::SessionId sessionId,uint userId);
     virtual int    _updateUserInfo(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    _getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue);
     virtual int    _getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo);
     virtual int    _getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo);
     virtual int    _setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue);
     virtual int    _setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName);
     virtual int    _deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup);
     virtual int    _deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList);

  protected:

     bool           mEnabled;
     Log            *mDebugLog;
     Log            *mProcessingLog;
};


}
}

