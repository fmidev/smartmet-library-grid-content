#pragma once

#include "../definition/ServiceInterface.h"

#include <map>


namespace SmartMet
{
namespace SessionManagement
{

// ====================================================================================
/*! \brief In-memory implementation of the SessionManagement ServiceInterface.
 *
 *  Stores sessions in a std::map protected by a ModificationLock.  A global
 *  instance (localSessionManagement) is provided for use within a single process. */
// ====================================================================================

class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     ~ServiceImplementation() override;

     void   shutdown() override;

  protected:

     int    _createSession(T::SessionId sessionId,SessionInfo& sessionInfo) override;
     int    _deleteSession(T::SessionId sessionId,T::SessionId requestedSessionId) override;
     int    _deleteExpiredSessions(T::SessionId sessionId) override;
     int    _updateSessionAccessTime(T::SessionId sessionId,T::SessionId requestedSessionId) override;
     int    _updateSessionInfo(T::SessionId sessionId,SessionInfo& sessionInfo) override;
     int    _getSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,std::string& attributeValue) override;
     int    _getSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList) override;
     int    _getSessionInfo(T::SessionId sessionId,T::SessionId requestedSessionId,SessionInfo& sessionInfo) override;
     int    _setSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName,const char *attributeValue) override;
     int    _setSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,AttributeList& attributeList) override;
     int    _deleteSessionAttribute(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,const char *attributeName) override;
     int    _deleteSessionAttributeGroup(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup) override;
     int    _deleteSessionAttributes(T::SessionId sessionId,T::SessionId requestedSessionId,const char *attributeGroup,std::vector<std::string>& attributeNameList) override;


     SessionInfo_sptr_map mSessions;          //!< In-memory map of active sessions keyed by session id.
     ModificationLock     mModificationLock;  //!< Lock protecting concurrent access to mSessions.
};

extern ServiceImplementation localSessionManagement;

}

}

