#include "ServerInfo.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"



namespace SmartMet
{
namespace T
{


ServerInfo::ServerInfo()
{
  try
  {
    mServerId = 0;
    mType = 0;
    mFlags = 0;
    mLastCall = time(0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInfo::ServerInfo(const char *csv)
{
  try
  {
    setCsv(csv);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInfo::ServerInfo(ServerInfo& serverInfo)
{
  try
  {
    mServerId = serverInfo.mServerId;
    mName = serverInfo.mName;
    mServerIor = serverInfo.mServerIor;
    mType = serverInfo.mType;
    mFlags = serverInfo.mFlags;
    mLastCall = serverInfo.mLastCall;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInfo::ServerInfo(std::string name,std::string serverIor,uint type)
{
  try
  {
    mServerId = 0;
    mName = name;
    mServerIor = serverIor;
    mType = type;
    mFlags = 0;
    mLastCall = time(0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInfo::~ServerInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServerInfo::operator=(ServerInfo& serverInfo)
{
  try
  {
    if (&serverInfo == this)
      return;

    mServerId = serverInfo.mServerId;
    mName = serverInfo.mName;
    mServerIor = serverInfo.mServerIor;
    mType = serverInfo.mType;
    mFlags = serverInfo.mFlags;
    mLastCall = serverInfo.mLastCall;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string ServerInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%s;%s;%u;%u",
        mServerId,
        mName.c_str(),
        mServerIor.c_str(),
        (uint)mType,
        mFlags);

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





std::string ServerInfo::getCsvHeader()
{
  try
  {
    std::string header = "serverId;name;serverIor;type;flags";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServerInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    char st[1000];
    strcpy(st,csv);

    uint c = 0;
    field[0] = st;
    char *p = st;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == ';'  ||  *p == '\n')
      {
        *p = '\0';
        p++;
        c++;
        field[c] = p;
      }
      else
      {
        p++;
      }
    }

    if (c >= 4)
    {
      mServerId = (uint)atoll(field[0]);
      mName = field[1];
      mServerIor = field[2];
      mType = (uint)atoll(field[3]);
      mFlags = (uint)atoll(field[4]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServerInfo::setCsv(std::string csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServerInfo* ServerInfo::duplicate()
{
  try
  {
    return new ServerInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServerInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ServerInfo\n";
    stream << space(level) << "- mServerId  = " << mServerId << "\n";
    stream << space(level) << "- mName      = " << mName << "\n";
    stream << space(level) << "- mServerIor = " << mServerIor << "\n";
    stream << space(level) << "- mType      = " << mType << "\n";
    stream << space(level) << "- mFlags     = " << mFlags << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}
