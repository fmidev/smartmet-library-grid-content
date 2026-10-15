#pragma once

#include "../../userManagement/definition/UserInfo.h"
#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/ModificationLock.h>
#include <string>



namespace SmartMet
{
namespace SessionManagement
{

using AttributeList = std::map<std::string,std::string>;           //!< Map of attribute name to value within one group.
using AttributeGroupList = std::map<std::string,AttributeList>;    //!< Map of attribute group name to its AttributeList.

// ====================================================================================
/*! \brief Record describing an active user session with its attributes and timeout.
 *
 *  Stores the session identifier, authentication credentials, network address,
 *  timing information (start, last access, timeout), status, a copy of the
 *  associated UserInfo, and a map of named attribute groups for arbitrary session state. */
// ====================================================================================

class SessionInfo
{
  public:

                        SessionInfo();
    virtual             ~SessionInfo();
    SessionInfo& operator=(const SessionInfo&) = default;

    AttributeGroupList& getAttributeGroupList();
    AttributeList*      getAttributeList(const char *attributeGroup);
    time_t              getExpirationTime() const;
    time_t              getLastAccessTime() const;
    T::SessionId        getSessionId() const;
    uchar               getStatus() const;
    uint                getTimeOutInSeconds() const;
    void                setTimeOutInSeconds(uint seconds);
    void                setSessionId(T::SessionId sessionId);
    void                setStatus(uchar status);
    const char*         getKey();
    void                setKey(const char *key);
    const char*         getAddress();
    void                setAddress(const char *address);

    bool                getAttribute(const char *attributeGroup,const char *attributeName,std::string& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,int& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,uint& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,Int64  & attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,UInt64 & attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,float& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,double& attributeValue);
    void                getAttributes(const char *attributeGroup,AttributeList& attributeList);
    void                setAttribute(const char *attributeGroup,const char *attributeName,const char *attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,int attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,uint attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,Int64 attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,UInt64 attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,double attributeValue);
    void                setAttributes(const char *attributeGroup,const AttributeList& attributeList);
    void                deleteAttribute(const char *attributeGroup,const char *attributeName);
    void                deleteAttributeGroup(const char *attributeGroup);
    void                deleteAttributes(const char *attributeGroup,const std::vector<std::string>& attributeNameList);
    void                updateAccessTime();
    void                updateSessionInfo(SessionInfo& sessionInfo);

    void                print(std::ostream& stream,uint level,uint optionFlags);

    UserManagement::UserInfo mUserInfo;  //!< Copy of the user record associated with this session.

  protected:

    T::SessionId        mSessionId;        //!< Unique identifier for this session.
    time_t              mStartTime;        //!< Epoch time when the session was created.
    time_t              mLastAccessTime;   //!< Epoch time of the most recent successful access.
    uint                mTimeOutInSeconds; //!< Inactivity timeout after which the session expires.
    std::string         mKey;              //!< Authentication key or token for this session.
    std::string         mAddress;          //!< Remote IP address of the session owner.
    uchar               mStatus;           //!< Session status code (active, expired, …).

    AttributeGroupList  mAttributeGroups;  //!< Named groups of key-value attributes for arbitrary session state.
    ModificationLock    mModificationLock; //!< Lock protecting concurrent attribute access.
};


using SessionInfo_sptr = std::shared_ptr<SessionInfo>;                          //!< Shared pointer to a SessionInfo record.
using SessionInfo_sptr_map = std::map<T::SessionId,SessionInfo_sptr>;           //!< Map from session id to SessionInfo shared pointer.



}
}
