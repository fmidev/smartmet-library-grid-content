#include "SessionInfo.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <macgyver/Exception.h>


namespace SmartMet
{
namespace SessionManagement
{

SessionInfo::SessionInfo()
{
  try
  {
    mSessionId = getTime() * rand();
    mStartTime = time(nullptr);
    mLastAccessTime = mStartTime;
    mTimeOutInSeconds = 600;
    mStatus = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



SessionInfo::~SessionInfo()
{
}



AttributeList* SessionInfo::getAttributeList(const char *attributeGroup)
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





AttributeGroupList& SessionInfo::getAttributeGroupList()
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




void SessionInfo::updateAccessTime()
{
  try
  {
    mLastAccessTime = time(nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void SessionInfo::updateSessionInfo(SessionInfo& sessionInfo)
{
  try
  {
    auto attrGroupList = sessionInfo.getAttributeGroupList();
    AutoWriteLock writeLock(&mModificationLock);

    mKey = sessionInfo.getKey();
    mAddress = sessionInfo.getAddress();
    mUserInfo = sessionInfo.mUserInfo;

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





time_t SessionInfo::getExpirationTime()
{
  try
  {
    return mLastAccessTime + mTimeOutInSeconds;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




time_t SessionInfo::getLastAccessTime()
{
  try
  {
    return mLastAccessTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




T::SessionId SessionInfo::getSessionId()
{
  try
  {
    return mSessionId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void SessionInfo::setSessionId(T::SessionId sessionId)
{
  try
  {
    mSessionId = sessionId;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





const char* SessionInfo::getKey()
{
  try
  {
    return mKey.c_str();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void SessionInfo::setKey(const char *key)
{
  try
  {
    mKey = key;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




const char* SessionInfo::getAddress()
{
  try
  {
    return mAddress.c_str();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void SessionInfo::setAddress(const char *address)
{
  try
  {
    mAddress = address;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uchar SessionInfo::getStatus()
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




uint SessionInfo::getTimeOutInSeconds()
{
  try
  {
    return mTimeOutInSeconds;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void SessionInfo::setTimeOutInSeconds(uint seconds)
{
  try
  {
    mTimeOutInSeconds = seconds;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void SessionInfo::setStatus(uchar status)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,std::string& attributeValue)
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





bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,int& attributeValue)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,uint& attributeValue)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,long long & attributeValue)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,unsigned long long& attributeValue)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,float& attributeValue)
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




bool SessionInfo::getAttribute(const char *attributeGroup,const char *attributeName,double& attributeValue)
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





void SessionInfo::getAttributes(const char *attributeGroup,AttributeList& attributeList)
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




void SessionInfo::setAttribute(const char *attributeGroup,const char *attributeName,const char *attributeValue)
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




void SessionInfo::setAttribute(const char *attributeGroup,const char *attributeName,int attributeValue)
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




void SessionInfo::setAttribute(const char *attributeGroup,const char *attributeName,uint attributeValue)
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




void SessionInfo::setAttribute(const char *attributeGroup,const char *attributeName,long long attributeValue)
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




void SessionInfo::setAttribute(const char *attributeGroup,const char *attributeName,double attributeValue)
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




void SessionInfo::setAttributes(const char *attributeGroup,AttributeList& attributeList)
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




void SessionInfo::deleteAttribute(const char *attributeGroup,const char *attributeName)
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





void SessionInfo::deleteAttributeGroup(const char *attributeGroup)
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





void SessionInfo::deleteAttributes(const char *attributeGroup,std::vector<std::string>& attributeNameList)
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





void SessionInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock readLock(&mModificationLock);

    stream << space(level) << "SessionInfo\n";
    stream << space(level) << "- mSessionId        = " << mSessionId << "\n";
    stream << space(level) << "- mStartTime        = " << mStartTime << "\n";
    stream << space(level) << "- mLastAccessTime   = " << mLastAccessTime << "\n";
    stream << space(level) << "- mTimeOutInSeconds = " << mTimeOutInSeconds << "\n";
    stream << space(level) << "- mStatus           = " << mStatus << "\n";
    stream << space(level) << "- mKey              = " << mKey << "\n";
    stream << space(level) << "- mAddress          = " << mAddress << "\n";
    stream << space(level) << "- mAttributeGroups  = " << mAttributeGroups.size() << " (items)\n";
    for (auto group = mAttributeGroups.begin(); group != mAttributeGroups.end(); ++group)
    {
      stream << space(level+2) << "* " << group->first << "\n";
      for (auto it = group->second.begin(); it != group->second.end(); ++it)
        stream << space(level+4) << "- " << it->first << " = " << it->second << "\n";
    }

    stream << space(level) << "- mUserInfo          = \n";
    mUserInfo.print(stream,level+2,optionFlags);

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







}
}
