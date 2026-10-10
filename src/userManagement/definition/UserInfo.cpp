#include "UserInfo.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <macgyver/Exception.h>


namespace SmartMet
{
namespace UserManagement
{

/*! \brief Constructor. */

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



/*! \brief Destructor. */

UserInfo::~UserInfo()
= default;



/*! \brief Get attribute list. */

AttributeList* UserInfo::getAttributeList(const char *attributeGroup)
{
  try
  {
    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return nullptr;

    return &(group->second);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Get attribute group list. */

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




/*! \brief Update user info. */

void UserInfo::updateUserInfo(UserInfo& sessionInfo)
{
  try
  {
    auto attrGroupList = sessionInfo.getAttributeGroupList();
    AutoWriteLock writeLock(&mModificationLock);

    for (auto& g : attrGroupList)
    {
      auto group = mAttributeGroups.find(g.first);
      if (group == mAttributeGroups.end())
      {
        mAttributeGroups.insert(std::pair<std::string,AttributeList>(g.first,g.second));
      }
      else
      {
        for (auto a = g.second.begin(); a != g.second.end(); ++a)
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





/*! \brief Get start time. */

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




/*! \brief Get end time. */

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




/*! \brief Get status. */

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




/*! \brief Get user id. */

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




/*! \brief Get full name. */

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




/*! \brief Get username. */

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




/*! \brief Get password. */

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




/*! \brief Set status. */

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




/*! \brief Set user id. */

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




/*! \brief Add permission. */

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




/*! \brief Clear permissions. */

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




/*! \brief Delete permission. */

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




/*! \brief Has permission. */

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




/*! \brief Add user group. */

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




/*! \brief Clear user groups. */

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





/*! \brief Delete user group. */

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




/*! \brief Is user group member. */

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





/*! \brief Get attribute. */

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





/*! \brief Get attribute. */

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




/*! \brief Get attribute. */

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




/*! \brief Get attribute. */

bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,Int64  & attributeValue)
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




/*! \brief Get attribute. */

bool UserInfo::getAttribute(const char *attributeGroup,const char *attributeName,UInt64 & attributeValue)
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




/*! \brief Get attribute. */

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




/*! \brief Get attribute. */

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





/*! \brief Get attributes. */

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

    for (auto& it : attributeList)
    {
      auto attr = group->second.find(it.first);
      if (attr != group->second.end())
        it.second = attr->second;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Set attribute. */

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




/*! \brief Set attribute. */

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




/*! \brief Set attribute. */

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




/*! \brief Set attribute. */

void UserInfo::setAttribute(const char *attributeGroup,const char *attributeName,Int64  attributeValue)
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




/*! \brief Set attribute. */

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




/*! \brief Set attributes. */

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

    for (auto& it : attributeList)
    {
      auto attr = group->second.find(it.first);
      if (attr != group->second.end())
        attr->second = it.second;
      else
        group->second.insert(std::pair<std::string,std::string>(it.first,it.second));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Delete attribute. */

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





/*! \brief Delete attribute group. */

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





/*! \brief Delete attributes. */

void UserInfo::deleteAttributes(const char *attributeGroup,std::vector<std::string>& attributeNameList)
{
  try
  {
    AutoWriteLock writeLock(&mModificationLock);

    auto group = mAttributeGroups.find(attributeGroup);
    if (group == mAttributeGroups.end())
      return;

    for (auto& it : attributeNameList)
    {
      group->second.erase(it);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Print. */

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
    for (auto& mAttributeGroup : mAttributeGroups)
    {
      stream << space(level+2) << "* " << mAttributeGroup.first << "\n";
      for (auto it = mAttributeGroup.second.begin(); it != mAttributeGroup.second.end(); ++it)
        stream << space(level+4) << "- " << it->first << " = " << it->second << "\n";
    }

    stream << space(level) << "- mUserGroups       = " << mUserGroups.size() << " (items)\n";
    for (const auto& mUserGroup : mUserGroups)
      stream << space(level+2) << "* " << mUserGroup << "\n";

    stream << space(level) << "- mUserPermissions  = " << mUserPermissions.size() << " (items)\n";
    for (const auto& mUserPermission : mUserPermissions)
      stream << space(level+2) << "* " << mUserPermission << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





}
}
