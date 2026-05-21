#pragma once

#include "SessionInfo.h"
#include "ServiceResults.h"
#include <grid-files/common/Log.h>

#include <vector>


namespace SmartMet
{
namespace SessionManagement
{

// ====================================================================================
/*! \brief Abstract base class for the SessionManagement service layer.
 *
 *  Declares CRUD operations on sessions and their attribute groups.  Concrete
 *  backends (ServiceImplementation, …) override the protected _-prefixed methods;
 *  the public methods add cross-cutting concerns before delegating. */
// ====================================================================================

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

     virtual int    createSession(T::SessionId sessionId,SessionInfo& sessionInfo);
     virtual int    deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId);
     virtual int    deleteExpiredSessions(T::SessionId sessionId);
     virtual int    updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId);
     virtual int    updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo);
     virtual int    getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue);
     virtual int    getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo);
     virtual int    setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue);
     virtual int    setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName);
     virtual int    deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup);
     virtual int    deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList);

  protected:

     virtual int    _createSession(T::SessionId sessionId,SessionInfo& sessionInfo);
     virtual int    _deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId);
     virtual int    _deleteExpiredSessions(T::SessionId sessionId);
     virtual int    _updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId);
     virtual int    _updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo);
     virtual int    _getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue);
     virtual int    _getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo);
     virtual int    _setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue);
     virtual int    _setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName);
     virtual int    _deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup);
     virtual int    _deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList);

  protected:

     bool           mEnabled;        //!< When false, all service calls return SERVICE_DISABLED immediately.
     Log            *mDebugLog;       //!< Optional log for verbose debug output.
     Log            *mProcessingLog;  //!< Optional log for request timing and statistics.
};


}
}

