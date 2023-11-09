#include "ServiceImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace SessionManagement
{

ServiceImplementation localSessionManagement;




ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ServiceImplementation::~ServiceImplementation()
{
}




void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_createSession(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    SessionInfo *session = new SessionInfo();
    sessionInfo.setSessionId(session->getSessionId());
    *session = sessionInfo;

    SessionInfo_sptr s(session);
    AutoWriteLock writeLock(&mModificationLock);
    mSessions.insert(std::pair<T::SessionId,SessionInfo_sptr>(session->getSessionId(),s));

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock writeLock(&mModificationLock);
    mSessions.erase(requestedSessionId);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteExpiredSessions(T::SessionId sessionId)
{
  FUNCTION_TRACE
  try
  {
    std::vector<T::SessionId> expiredSessions;
    time_t currentTime = time(0);

    {
      AutoReadLock readLock(&mModificationLock);
      for (auto session = mSessions.begin(); session != mSessions.end(); ++session)
      {
        if (session->second->getExpirationTime() < currentTime)
          expiredSessions.push_back(session->first);
      }

      if (expiredSessions.size() == 0)
        return Result::OK;
    }

    AutoWriteLock writeLock(&mModificationLock);

    for (auto it = expiredSessions.begin(); it != expiredSessions.end(); ++it)
    {
      mSessions.erase(*it);
    }

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->updateAccessTime();

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(sessionInfo.getSessionId());
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->updateSessionInfo(sessionInfo);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    if (session->second->getAttribute(attributeGroup,attributeName,attributeValue))
      return Result::OK;

    return Result::SESSION_ATTRIBUTE_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->getAttributes(attributeGroup,attributeList);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    sessionInfo = *(session->second);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->setAttribute(attributeGroup,attributeName,attributeValue);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->setAttributes(attributeGroup,attributeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->deleteAttribute(attributeGroup,attributeName);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->deleteAttributeGroup(attributeGroup);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto session = mSessions.find(requestedSessionId);
    if (session == mSessions.end())
      return Result::DATA_NOT_FOUND;

    session->second->deleteAttributes(attributeGroup,attributeNameList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}

