#include "ServiceInterface.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace SessionManagement
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





int ServiceInterface::createSession(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _createSession(sessionId,sessionInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu);result %d;time %f;",__FUNCTION__,sessionId,sessionInfo.getSessionId(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteSession(sessionId,requestedSessionId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteExpiredSessions(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteExpiredSessions(sessionId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _updateSessionAccessTime(sessionId,requestedSessionId);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _updateSessionInfo(sessionId,sessionInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,SessionInfo(%llu));result %d;time %f;",__FUNCTION__,sessionId,sessionInfo.getSessionId(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getSessionAttribute(sessionId,requestedSessionId,attributeGroup,attributeName,attributeValue);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeName,attributeValue.c_str(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getSessionAttributes(sessionId,requestedSessionId,attributeGroup,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,Attribute[%ld]);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getSessionInfo(sessionId,requestedSessionId,sessionInfo);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _setSessionAttribute(sessionId,requestedSessionId,attributeGroup,attributeName,attributeValue);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeName,attributeValue,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _setSessionAttributes(sessionId,requestedSessionId,attributeGroup,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,Attribute[%ld]);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteSessionAttribute(sessionId,requestedSessionId,attributeGroup,attributeName);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,%s);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeName,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceInterface::deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteSessionAttributeGroup(sessionId,requestedSessionId,attributeGroup);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _deleteSessionAttributes(sessionId,requestedSessionId,attributeGroup,attributeNameList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%llu,%s,attributeName[%ld]);result %d;time %f;",__FUNCTION__,sessionId,requestedSessionId,attributeGroup,attributeNameList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_createSession(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteExpiredSessions(T::SessionId sessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



int ServiceInterface::_deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}


int ServiceInterface::_deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



}
}

