#pragma once

#include "../definition/ServiceInterface.h"

#include <map>
#include <set>


namespace SmartMet
{
namespace UserManagement
{

typedef std::set<std::string> GroupPermissions;
typedef std::map<std::string,GroupPermissions> GroupPermissionList;


class ServiceImplementation : public ServiceInterface
{
  public:
                    ServiceImplementation();
     virtual        ~ServiceImplementation();

     virtual void   shutdown();
     virtual void   init(const char *groupFilename,const char *userFilename);
     virtual void   update();

  protected:

     virtual int    _addUser(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    _deleteUser(T::SessionId sessionId,uint userId);
     virtual int    _updateUserInfo(T::SessionId sessionId,UserInfo& userInfo);
     virtual int    _getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue);
     virtual int    _getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo);
     virtual int    _getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo);
     virtual int    _setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue);
     virtual int    _setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList);
     virtual int    _deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName);
     virtual int    _deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup);
     virtual int    _deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList);

     virtual void   updateGroups();
     virtual void   updateUsers();

     time_t                     mLastUpdateCheckTime;
     time_t                     mGroups_modificationTime;
     std::string                mGroupFilename;
     GroupPermissionList        mGroups;
     time_t                     mUsers_modificationTime;
     std::string                mUserFilename;
     UserInfo_sptr_map          mUsers;
     std::map<std::string,uint> mUsernameToUserId;
     ModificationLock           mModificationLock;

};

extern ServiceImplementation localUserManagement;

}

}

