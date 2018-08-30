#include "ServerInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>


namespace SmartMet
{
namespace T
{


ServerInfoList::ServerInfoList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ServerInfoList::ServerInfoList(ServerInfoList& serverInfoList)
{
  try
  {
    serverInfoList.lock();
    uint sz = serverInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = serverInfoList.getServerInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.push_back(info->duplicate());
    }
    serverInfoList.unlock();
  }
  catch (...)
  {
    serverInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ServerInfoList::~ServerInfoList()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ServerInfoList& ServerInfoList::operator=(ServerInfoList& serverInfoList)
{
  try
  {
    if (&serverInfoList == this)
      return *this;

    clear();

    AutoThreadLock lock(&mThreadLock);
    serverInfoList.lock();
    uint sz = serverInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = serverInfoList.getServerInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.push_back(info->duplicate());
    }
    serverInfoList.unlock();
    return *this;
  }
  catch (...)
  {
    serverInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServerInfoList::addServerInfo(ServerInfo* serverInfo)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    mList.push_back(serverInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServerInfoList::clear()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





bool ServerInfoList::deleteServerInfoById(uint serverId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = getServerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mServerId == serverId)
      {
        delete mList[t];
        mList.erase(mList.begin() + t);
        return true;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ServerInfo* ServerInfoList::getPreviousServerInfoById(uint serverId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    uint closestId = 0;
    uint closestIndex = 0;
    uint maxId = 0;
    uint maxIndex = 0;


    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = getServerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mServerId != serverId)
      {
        if (info->mServerId < serverId  &&  info->mServerId > closestId)
        {
          closestId = info->mServerId;
          closestIndex = t;
        }

        if (info->mServerId > maxId)
        {
          maxId = info->mServerId;
          maxIndex = t;
        }
      }
    }

    if (closestId != 0)
      return getServerInfoByIndexNoCheck(closestIndex);

    if (maxId != 0)
      return getServerInfoByIndexNoCheck(maxIndex);

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ServerInfo* ServerInfoList::getServerInfoById(uint serverId)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = getServerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mServerId == serverId)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ServerInfo* ServerInfoList::getServerInfoByIndex(uint index)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    if (index >= mList.size())
      return nullptr;

    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ServerInfo* ServerInfoList::getServerInfoByIndexNoCheck(uint index)
{
  try
  {
    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ServerInfo*  ServerInfoList::getServerInfoByIor(std::string serverIor)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = getServerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  info->mServerIor == serverIor)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ServerInfo*  ServerInfoList::getServerInfoByName(std::string serverName)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    uint sz = getLength();
    for (uint t=0; t<sz; t++)
    {
      ServerInfo *info = getServerInfoByIndexNoCheck(t);
      if (info != nullptr  &&  strcasecmp(info->mName.c_str(),serverName.c_str()) == 0)
        return info;
    }
    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint ServerInfoList::getLength()
{
  try
  {
    return mList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInfoList::lock()
{
  try
  {
    return mThreadLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInfoList::unlock()
{
  try
  {
    return mThreadLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInfoList::writeToFile(std::string filename)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
    {
      fprintf(file,"%s\n",mList[t]->getCsv().c_str());
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ServerInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    stream << space(level) << "ServerInfoList\n";
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      mList[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
