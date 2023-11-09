#include "UserInfo.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <macgyver/Exception.h>


namespace SmartMet
{
namespace UserManagement
{

UserInfo::UserInfo()
{
  try
  {
    mUserId = 0;
    mStartTime = 0;
    mEndTime = 0;
    mStatus = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



UserInfo::~UserInfo()
{
}



AttributeList* UserInfo::getAttributeList(const char *attributeGroup)
{
  try
  {
    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return NULL;

    return &(group->second);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





AttributeGroupList& UserInfo::getAttributeGroupList()
{
  try
  {
    return mAttributeGroups;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::updateUserInfo(UserInfo& sessionInfo)
{
  try
  {
    auto attrGroupList = sessionInfo.getAttributeGroupList();
    AutoWriteLock writeLock(&mModificationLock);

    for (auto g = attrGroupList.begin(); g != attrGroupList.end(); ++g)
    {
      auto group = mAttributeGroups.find(g->first);
      if (group == mAttributeGroups.end())
      {
        mAttributeGroups.insert(std::pair<std::string,AttributeList>(g->first,g->second));
      }
      else
      {
        for (auto a = g->second.begin(); a != g->second.end(); ++a)
        {

          auto attr = group->second.find(a->first);
          if (attr != group->second.end())
          {
            if (attr->second != a->second)
              attr->second = a->second;
          }
          else
          {
            group->second.insert(std::pair<std::string,std::string>(a->first,a->second));
          }
        }
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





time_t UserInfo::getStartTime()
{
  try
  {
    return mStartTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




time_t UserInfo::getEndTime()
{
  try
  {
    return mEndTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uchar UserInfo::getStatus()
{
  try
  {
    return mStatus;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint UserInfo::getUserId()
{
  try
  {
    return mUserId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




const char* UserInfo::getFullName()
{
  try
  {
    return mFullName.c_str();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo:: setFullName(const char *fullName)
{
  try
  {
    mFullName = fullName;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




const char* UserInfo::getUsername()
{
  try
  {
    return mUsername.c_str();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo:: setUsername(const char *username)
{
  try
  {
    mUsername = username;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




const char* UserInfo::getPassword()
{
  try
  {
    return mPassword.c_str();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo:: setPassword(const char *password)
{
  try
  {
    mPassword = password;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setStatus(uchar status)
{
  try
  {
    mStatus = status;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setUserId(uint userId)
{
  try
  {
    mUserId = userId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::addPermission(const char *permission)
{
  try
  {
    mUserPermissions.insert(permission);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::clearPermissions()
{
  try
  {
    mUserPermissions.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::deletePermission(const char *permission)
{
  try
  {
    mUserPermissions.erase(permission);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::hasPermission(const char *permission)
{
  try
  {
    if (mUserPermissions.find(permission) == mUserPermissions.end())
      return false;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::addUserGroup(const char *userGroup)
{
  try
  {
    mUserGroups.insert(userGroup);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::clearUserGroups()
{
  try
  {
    mUserGroups.clear();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo::deleteUserGroup(const char *userGroup)
{
  try
  {
    mUserGroups.erase(userGroup);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::isUserGroupMember(const char *userGroup)
{
  try
  {
    if (mUserGroups.find(userGroup) == mUserGroups.end())
      return false;

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,std::string& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = attr->second;
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,int& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toInt32(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,uint& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toUInt32(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,long long & attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toInt64(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,unsigned long long& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toUInt64(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,float& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toFloat(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,double& attributeValue)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return false;

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attributeValue = toDouble(attr->second);
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo::getAttributes(const char *attributeGroup,AttributeList& attributeList)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return;

    if (attributeList.size() == 0)
    {
      attributeList = group->second;
      return;
    }

    for (auto it = attributeList.begin(); it != attributeList.end(); ++it)
    {
      auto attr = group->second.find(it->first);
      if (attr != group->second.end())
        it->second = attr->second;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,const char *attributeValue)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      AttributeList list;
      list.insert(std::pair<std::string,std::string>(attributeName,attributeValue));
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,list));
      return;
    }

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attr->second = attributeValue;
      return;
    }
    group->second.insert(std::pair<std::string,std::string>(attributeName,attributeValue));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,int attributeValue)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      AttributeList list;
      list.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,list));
      return;
    }

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attr->second = std::to_string(attributeValue);
      return;
    }
    group->second.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,uint attributeValue)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      AttributeList list;
      list.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,list));
      return;
    }

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attr->second = std::to_string(attributeValue);
      return;
    }
    group->second.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,long long attributeValue)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      AttributeList list;
      list.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,list));
      return;
    }

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attr->second = std::to_string(attributeValue);
      return;
    }
    group->second.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,double attributeValue)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      AttributeList list;
      list.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,list));
      return;
    }

    auto attr = group->second.find(attributeName);
    if (attr != group->second.end())
    {
      attr->second = std::to_string(attributeValue);
      return;
    }
    group->second.insert(std::pair<std::string,std::string>(attributeName,std::to_string(attributeValue)));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::setAttributes(const char *attributeGroup,AttributeList& attributeList)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
    {
      mAttributeGroups.insert(std::pair<std::string,AttributeList>(attributeGroup,attributeList));
      return;
    }

    for (auto it = attributeList.begin(); it != attributeList.end(); ++it)
    {
      auto attr = group->second.find(it->first);
      if (attr != group->second.end())
        attr->second = it->second;
      else
        group->second.insert(std::pair<std::string,std::string>(it->first,it->second));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void UserInfo::deleteAttribute(const char *attributeGroup,const char *attributeName)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return;

    group->second.erase(attributeName);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo::deleteAttributeGroup(const char *attributeGroup)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);
    mAttributeGroups.erase(attributeGroup);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo::deleteAttributes(const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return;

    for (auto it = attributeNameList.begin(); it != attributeNameList.end(); ++it)
    {
      group->second.erase(*it);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void UserInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    stream << space(level) << "UserInfo\n";
    stream << space(level) << "- mUserId           = " << mUserId << "\n";
    stream << space(level) << "- mUsername         = " << mUsername << "\n";
    stream << space(level) << "- mFullName         = " << mFullName << "\n";
    stream << space(level) << "- mPassword         = " << mPassword << "\n";
    stream << space(level) << "- mStartTime        = " << mStartTime << "\n";
    stream << space(level) << "- mEndTime          = " << mEndTime << "\n";
    stream << space(level) << "- mStatus           = " << mStatus << "\n";
    stream << space(level) << "- mAttributeGroups  = " << mAttributeGroups.size() << " (items)\n";
    for (auto group = mAttributeGroups.begin(); group != mAttributeGroups.end(); ++group)
    {
      stream << space(level+2) << "* " << group->first << "\n";
      for (auto it = group->second.begin(); it != group->second.end(); ++it)
        stream << space(level+4) << "- " << it->first << " = " << it->second << "\n";
    }

    stream << space(level) << "- mUserGroups       = " << mUserGroups.size() << " (items)\n";
    for (auto g = mUserGroups.begin(); g != mUserGroups.end(); ++g)
      stream << space(level+2) << "* " << *g << "\n";

    stream << space(level) << "- mUserPermissions  = " << mUserPermissions.size() << " (items)\n";
    for (auto p = mUserPermissions.begin(); p != mUserPermissions.end(); ++p)
      stream << space(level+2) << "* " << *p << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





}
}
