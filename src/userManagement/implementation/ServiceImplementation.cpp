#include "ServiceImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace UserManagement
{


ServiceImplementation localUserManagement;



ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mLastUpdateCheckTime = 0;
    mGroups_modificationTime = 0;
    mUsers_modificationTime = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ServiceImplementation::~ServiceImplementation()
{
}





void ServiceImplementation::init(const char *groupFilename,const char *userFilename)
{
  FUNCTION_TRACE
  try
  {
    mGroupFilename = groupFilename;
    mUserFilename = userFilename;
    update();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::update()
{
  FUNCTION_TRACE
  try
  {
    time_t currentTime = time(nullptr);

    if ((currentTime-mLastUpdateCheckTime) > 30)
    {
      updateGroups();
      updateUsers();
      mLastUpdateCheckTime = currentTime;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::updateGroups()
{
  FUNCTION_TRACE
  try
  {
    if (mGroupFilename.empty())
      return;

    time_t mt = getFileModificationTime(mGroupFilename.c_str());
    if (mt == mGroups_modificationTime)
      return;

    mGroups_modificationTime = mt;
    mUsers_modificationTime = 0;

    FILE *file = fopen(mGroupFilename.c_str(),"re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot open the group file!");
      exception.addParameter("Filename",mGroupFilename);
      throw exception;
    }

    {
      AutoWriteLock writeLock(&mModificationLock);
      mGroups.clear();
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
          {
            *p = '\0';
            p++;
            field[c] = p;
            c++;
          }
          else
          {
            p++;
          }
        }

        if (c > 1)
        {
          std::string name;
          GroupPermissions permissions;

          if (field[0][0] != '\0')
            name = field[0];

          if (field[1][0] != '\0')
            splitString(field[1],',',permissions);

          AutoWriteLock writeLock(&mModificationLock);
          mGroups.insert(std::pair<std::string,GroupPermissions>(name,permissions));
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void ServiceImplementation::updateUsers()
{
  FUNCTION_TRACE
  try
  {
    if (mUserFilename.empty())
      return;

    time_t mt = getFileModificationTime(mUserFilename.c_str());
    if (mt == mUsers_modificationTime)
      return;

    mUsers_modificationTime = mt;

    FILE *file = fopen(mUserFilename.c_str(),"re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot open the users file!");
      exception.addParameter("Filename",mUserFilename);
      throw exception;
    }

    {
      AutoWriteLock writeLock(&mModificationLock);
      mUsernameToUserId.clear();
      mUsers.clear();
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
          {
            *p = '\0';
            p++;
            field[c] = p;
            c++;
          }
          else
          {
            p++;
          }
        }

        if (c > 3)
        {
          UserInfo *user = new UserInfo();

          if (field[0][0] != '\0')
            user->setUserId(atoi(field[0]));

          if (field[1][0] != '\0')
            user->setUsername(field[1]);

          if (field[2][0] != '\0')
            user->setPassword(field[2]);

          if (field[3][0] != '\0')
            user->setFullName(field[3]);

          if (field[4][0] != '\0')
          {
            std::vector<std::string> list;
            splitString(field[4],',',list);
            for (auto it=list.begin(); it!=list.end();++it)
            {
              user->addUserGroup(it->c_str());

              auto group = mGroups.find(*it);
              if (group != mGroups.end())
              {
                for (auto p=group->second.begin(); p!=group->second.end();++p)
                  user->addPermission(p->c_str());
              }
            }
          }

          UserInfo_sptr u(user);
          AutoWriteLock writeLock(&mModificationLock);
          mUsers.insert(std::pair<uint,UserInfo_sptr>(u->getUserId(),u));
          mUsernameToUserId.insert(std::pair<std::string,uint>(u->getUsername(),u->getUserId()));
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceImplementation::shutdown()
{
}





int ServiceImplementation::_addUser(T::SessionId sessionId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    UserInfo *user = new UserInfo();
    *user = userInfo;

    UserInfo_sptr u(user);
    AutoWriteLock writeLock(&mModificationLock);
    mUsers.insert(std::pair<uint,UserInfo_sptr>(u->getUserId(),u));

    mUsernameToUserId.insert(std::pair<std::string,uint>(u->getUsername(),u->getUserId()));

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteUser(T::SessionId sessionId,uint userId)
{
  FUNCTION_TRACE
  try
  {
    AutoWriteLock writeLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    mUsernameToUserId.erase(user->second->getUsername());
    mUsers.erase(userId);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_updateUserInfo(T::SessionId sessionId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userInfo.getUserId());
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->updateUserInfo(userInfo);

    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceImplementation::_getUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    if (user->second->getAttribute(attributeGroup,attributeName,attributeValue))
      return Result::OK;

    return Result::USER_ATTRIBUTE_NOT_FOUND;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->getAttributes(attributeGroup,attributeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getUserInfo(T::SessionId sessionId,uint userId,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    update();

    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userInfo.getUserId());
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    userInfo = *(user->second);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_getUserInfoByUsername(T::SessionId sessionId,const char *username,UserInfo& userInfo)
{
  FUNCTION_TRACE
  try
  {
    update();

    AutoReadLock readLock(&mModificationLock);

    auto res = mUsernameToUserId.find(username);
    if (res == mUsernameToUserId.end())
      return Result::DATA_NOT_FOUND;

    auto user = mUsers.find(res->second);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    userInfo = *(user->second);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_setUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->setAttribute(attributeGroup,attributeName,attributeValue);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_setUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->setAttributes(attributeGroup,attributeList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ServiceImplementation::_deleteUserAttribute(T::SessionId sessionId,uint userId,const char *attributeGroup,const char *attributeName)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->deleteAttribute(attributeGroup,attributeName);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteUserAttributeGroup(T::SessionId sessionId,uint userId,const char *attributeGroup)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->deleteAttributeGroup(attributeGroup);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceImplementation::_deleteUserAttributes(T::SessionId sessionId,uint userId,const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  FUNCTION_TRACE
  try
  {
    AutoReadLock readLock(&mModificationLock);
    auto user = mUsers.find(userId);
    if (user == mUsers.end())
      return Result::DATA_NOT_FOUND;

    user->second->deleteAttributes(attributeGroup,attributeNameList);
    return Result::OK;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





}

}

