#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/ModificationLock.h>
#include <string>



namespace SmartMet
{
namespace UserManagement
{

typedef std::map<std::string,std::string> AttributeList;           //!< Map of attribute name to value within one group.
typedef std::map<std::string,AttributeList> AttributeGroupList;    //!< Map of attribute group name to its AttributeList.

// ====================================================================================
/*! \brief Record describing a registered user with credentials, groups, permissions, and attributes.
 *
 *  Stores identity information (username, full name, password hash), membership in
 *  named user groups, a set of permission strings, validity window (start/end times),
 *  status, and a map of named attribute groups for arbitrary extensible user state. */
// ====================================================================================

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
    bool                getAttribute(const char *attributeGroup,const char *attributeName,Int64  & attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,UInt64 & attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,float& attributeValue);
    bool                getAttribute(const char *attributeGroup,const char *attributeName,double& attributeValue);
    void                getAttributes(const char *attributeGroup,AttributeList& attributeList);
    void                setAttribute(const char *attributeGroup,const char *attributeName,const char *attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,int attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,uint attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,Int64  attributeValue);
    void                setAttribute(const char *attributeGroup,const char *attributeName,double attributeValue);
    void                setAttributes(const char *attributeGroup,AttributeList& attributeList);
    void                deleteAttribute(const char *attributeGroup,const char *attributeName);
    void                deleteAttributeGroup(const char *attributeGroup);
    void                deleteAttributes(const char *attributeGroup,std::vector<std::string>& attributeNameList);
    void                updateUserInfo(UserInfo& sessionInfo);

    void                print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    uint                  mUserId;            //!< Unique numeric identifier for this user.
    std::string           mUsername;          //!< Login name (unique within the user store).
    std::string           mPassword;          //!< Password hash or credential string.
    std::string           mFullName;          //!< Human-readable display name.
    std::set<std::string> mUserGroups;        //!< Set of group names this user belongs to.
    std::set<std::string> mUserPermissions;   //!< Set of permission strings granted to this user.
    time_t                mStartTime;         //!< Epoch time from which this user account is valid.
    time_t                mEndTime;           //!< Epoch time after which this user account expires (0 = never).
    uchar                 mStatus;            //!< Account status code (active, locked, …).

    AttributeGroupList    mAttributeGroups;   //!< Named groups of key-value attributes for extensible user data.
    ModificationLock      mModificationLock;  //!< Lock protecting concurrent attribute access.
};


typedef std::shared_ptr<UserInfo> UserInfo_sptr;                    //!< Shared pointer to a UserInfo record.
typedef std::map<uint,UserInfo_sptr> UserInfo_sptr_map;             //!< Map from user id to UserInfo shared pointer.



}
}
