#pragma once

#include "../../userManagement/definition/UserInfo.h"
#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/ModificationLock.h>
#include <string>



namespace SmartMet
{
namespace SessionManagement
{

typedef std::map<std::string,std::string> AttributeList;
typedef std::map<std::string,AttributeList> AttributeGroupList;


class SessionInfo
{
  public:

                        SessionInfo();
    virtual             ~SessionInfo();

    AttributeGroupList& getAttributeGroupList();
    AttributeList*      getAttributeList(const char *attributeGroup);
    time_t              getExpirationTime();
    time_t              getLastAccessTime();
    T::SessionId        getSessionId();
    uchar               getStatus();
    uint                getTimeOutInSeconds();
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
    bool                getAttribute(const char *attributeGroup,const char *attributeName,long long & attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,unsigned long long& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,float& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,double& attributeValue);
    void                getAttributes(const char *attributeGroup,AttributeList& attributeList);
    void                setAttribute(const char *attributeGroup,const char *attributeName,const char *attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,int attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,uint attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,long long attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,double attributeValue);
    void                setAttributes(const char *attributeGroup,AttributeList& attributeList);
    void                deleteAttribute(const char *attributeGroup,const char *attributeName);
    void                deleteAttributeGroup(const char *attributeGroup);
    void                deleteAttributes(const char *attributeGroup,std::vector<std::string>& attributeNameList);
    void                updateAccessTime();
    void                updateSessionInfo(SessionInfo& sessionInfo);

    void                print(std::ostream& stream,uint level,uint optionFlags);

    UserManagement::UserInfo mUserInfo;

  protected:

    T::SessionId        mSessionId;
    time_t              mStartTime;
    time_t              mLastAccessTime;
    uint                mTimeOutInSeconds;
    std::string         mKey;
    std::string         mAddress;
    uchar               mStatus;

    AttributeGroupList  mAttributeGroups;
    ModificationLock    mModificationLock;
};


typedef std::shared_ptr<SessionInfo> SessionInfo_sptr;
typedef std::map<T::SessionId,SessionInfo_sptr> SessionInfo_sptr_map;



}
}
