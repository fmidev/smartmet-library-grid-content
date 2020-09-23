#include "ServerInfo.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>



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
    mLastCall = time(nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServerInfo::ServerInfo(const ServerInfo& serverInfo)
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    mLastCall = time(nullptr);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServerInfo::~ServerInfo()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





ServerInfo& ServerInfo::operator=(const ServerInfo& serverInfo)
{
  try
  {
    if (&serverInfo == this)
      return *this;

    mServerId = serverInfo.mServerId;
    mName = serverInfo.mName;
    mServerIor = serverInfo.mServerIor;
    mType = serverInfo.mType;
    mFlags = serverInfo.mFlags;
    mLastCall = serverInfo.mLastCall;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
        mType,
        mFlags);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      mServerId = toInt64(field[0]);
      mName = field[1];
      mServerIor = field[2];
      mType = toInt64(field[3]);
      mFlags = toInt64(field[4]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}
