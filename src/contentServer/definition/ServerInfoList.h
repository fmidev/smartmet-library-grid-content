#pragma once

#include "ServerInfo.h"

#include <grid-files/common/ThreadLock.h>
#include <vector>


namespace SmartMet
{
namespace T
{

typedef std::vector<ServerInfo*> ServerInfo_pvec;


class ServerInfoList
{
  public:
                    ServerInfoList();
                    ServerInfoList(ServerInfoList& serverInfoList);
    virtual         ~ServerInfoList();

    ServerInfoList& operator=(ServerInfoList& serverInfoList);

    void            addServerInfo(ServerInfo *serverInfo);
    void            clear();
    bool            deleteServerInfoById(uint serverId);
    ServerInfo*     getServerInfoById(uint serverId);
    ServerInfo*     getServerInfoByIndex(uint index);
    ServerInfo*     getServerInfoByIndexNoCheck(uint index);
    ServerInfo*     getServerInfoByIor(std::string serverIor);
    ServerInfo*     getServerInfoByName(std::string serverName);
    ServerInfo*     getPreviousServerInfoById(uint serverId);
    uint            getLength();
    void            lock();
    void            unlock();
    void            print(std::ostream& stream,uint level,uint optionFlags);
    void            writeToFile(std::string filename);

  protected:

    ThreadLock      mThreadLock;
    ServerInfo_pvec mList;
};



}
}
