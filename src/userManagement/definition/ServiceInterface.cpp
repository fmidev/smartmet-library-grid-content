#include "ServiceInterface.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <memory>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace UserManagement
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mEnabled = true;
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



ServiceInterface::~ServiceInterface()
{
}



void ServiceInterface::shutdown()
{
}




Log* ServiceInterface::getDebugLog()
{
  FUNCTION_TRACE
  try
  {
    return mDebugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




Log* ServiceInterface::getProcessingLog()
{
  FUNCTION_TRACE
  try
  {
    return mProcessingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLog)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLog = processingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setDebugLog(Log *debugLog)
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = debugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mEnabled = enabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool ServiceInterface::isEnabled()
{
  FUNCTION_TRACE
  try
  {
    return mEnabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::addUser(T::SessionId sessionId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _addUser(sessionId,userInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,userInfo.getUserId(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteUser(T::SessionId sessionId,uint userId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteUser(sessionId,userId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,userId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::updateUserInfo(T::SessionId sessionId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _updateUserInfo(sessionId,userInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,userInfo.getUserId(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getUserAttribute(sessionId,userId,attributeGroup,attributeName,attributeValue);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeName,attributeValue.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getUserAttributes(sessionId,userId,attributeGroup,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,Attribute[%ld]);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getUserInfo(sessionId,userId,userInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,userId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getUserInfoByUsername(sessionId,username,userInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,username,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _setUserAttribute(sessionId,userId,attributeGroup,attributeName,attributeValue);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeName,attributeValue,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _setUserAttributes(sessionId,userId,attributeGroup,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,Attribute[%ld]);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteUserAttribute(sessionId,userId,attributeGroup,attributeName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeName,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteUserAttributeGroup(sessionId,userId,attributeGroup);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteUserAttributes(sessionId,userId,attributeGroup,attributeNameList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%s,AttributeName[%ld]);result %d;time %f;",__FUNCTION__,sessionId,userId,attributeGroup,attributeNameList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_addUser(T::SessionId sessionId,UserInfo& userInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteUser(T::SessionId sessionId,uint userId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_updateUserInfo(T::SessionId sessionId,UserInfo& userInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




}
}

