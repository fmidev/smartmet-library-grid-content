#pragma once

#include "../definition/ServiceInterface.h"

#include <map>


namespace SmartMet
{
namespace SessionManagement
{

class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   shutdown();

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


     SessionInfo_sptr_map mSessions;
     ModificationLock     mModificationLock;
};

extern ServiceImplementation localSessionManagement;

}

}

