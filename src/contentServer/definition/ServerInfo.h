#pragma once

#include <string>
#include <time.h>



namespace SmartMet
{
namespace T
{


class ServerInfo
{
  public:
                  ServerInfo();
                  ServerInfo(ServerInfo& serverInfo);
                  ServerInfo(std::string name,std::string serverIor,uint type);
                  ServerInfo(const char *csv);
     virtual      ~ServerInfo();

     std::string  getCsv();
     std::string  getCsvHeader();
     void         setCsv(const char *csv);
     void         setCsv(std::string csv);

     void         operator=(ServerInfo& serverInfo);
     ServerInfo*  duplicate();
     void         print(std::ostream& stream,uint level,uint optionFlags);

     uint         mServerId;
     std::string  mName;
     std::string  mServerIor;
     uint         mType;
     uint         mFlags;
     time_t       mLastCall;
};



}
}
