#pragma once

#include "../definition/ServiceInterface.h"

#include <map>
#include <set>


namespace SmartMet
{
namespace UserManagement
{

typedef std::set<std::string> GroupPermissions;              //!< Set of permission strings for one user group.
typedef std::map<std::string,GroupPermissions> GroupPermissionList;  //!< Map from group name to its GroupPermissions.

// ====================================================================================
/*! \brief In-memory implementation of the UserManagement ServiceInterface.
 *
 *  Loads user and group definitions from files and stores them in memory.  The
 *  update() method hot-reloads changed files.  A global instance
 *  (localUserManagement) is provided for single-process use. */
// ====================================================================================

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

     time_t                     mLastUpdateCheckTime;       //!< Epoch time of the last file-modification check.
     time_t                     mGroups_modificationTime;   //!< Mtime of the group file at the last successful load.
     std::string                mGroupFilename;             //!< Path to the group definitions file.
     GroupPermissionList        mGroups;                    //!< Loaded group-to-permissions map.
     time_t                     mUsers_modificationTime;    //!< Mtime of the user file at the last successful load.
     std::string                mUserFilename;              //!< Path to the user definitions file.
     UserInfo_sptr_map          mUsers;                     //!< In-memory map of users keyed by user id.
     std::map<std::string,uint> mUsernameToUserId;          //!< Secondary index from username to user id.
     ModificationLock           mModificationLock;          //!< Lock protecting concurrent access to mUsers and mGroups.

};

extern ServiceImplementation localUserManagement;

}

}

