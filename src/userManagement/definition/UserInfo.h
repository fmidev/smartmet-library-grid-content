#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/ModificationLock.h>
#include <string>



namespace SmartMet
{
namespace UserManagement
{

typedef std::map<std::string,std::string> AttributeList;
typedef std::map<std::string,AttributeList> AttributeGroupList;


class UserInfo
{
  public:

                        UserInfo();
    virtual             ~UserInfo();

    AttributeGroupList& getAttributeGroupList();
    AttributeList*      getAttributeList(const char *attributeGroup);
    time_t              getStartTime();
    time_t              getEndTime();
    uchar               getStatus();
    uint                getUserId();
    void                setStatus(uchar status);
    void                setUserId(uint userId);
    void                setFullName(const char *fullName);
    const char*         getFullName();
    void                setUsername(const char *username);
    const char*         getUsername();
    void                setPassword(const char *password);
    const char*         getPassword();

    void                addPermission(const char *permission);
    void                clearPermissions();
    void                deletePermission(const char *permission);
    bool                hasPermission(const char *permission);

    void                addUserGroup(const char *userGroup);
    void                clearUserGroups();
    void                deleteUserGroup(const char *userGroup);
    bool                isUserGroupMember(const char *userGroup);

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
    void                updateUserInfo(UserInfo& sessionInfo);

    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    uint                  mUserId;
    std::string           mUsername;
    std::string           mPassword;
    std::string           mFullName;
    std::set<std::string> mUserGroups;
    std::set<std::string> mUserPermissions;
    time_t                mStartTime;
    time_t                mEndTime;
    uchar                 mStatus;

    AttributeGroupList    mAttributeGroups;
    ModificationLock      mModificationLock;
};


typedef std::shared_ptr<UserInfo> UserInfo_sptr;
typedef std::map<uint,UserInfo_sptr> UserInfo_sptr_map;



}
}
