#pragma once

#include "../../definition/ContentInfo.h"
#include "../../definition/ContentInfoList.h"
#include "../../definition/FileInfo.h"
#include "../../definition/FileAndContent.h"
#include "../../definition/FileInfoList.h"
#include "../../definition/EventInfo.h"
#include "../../definition/EventInfoList.h"
#include "../../definition/ServerInfo.h"
#include "../../definition/ServerInfoList.h"
#include "../../definition/ProducerInfo.h"
#include "../../definition/ProducerInfoList.h"
#include "../../definition/GenerationInfo.h"
#include "../../definition/GenerationInfoList.h"
#include "../../definition/ForecastTime.h"

#include "../stubs/ContentServer_serviceInterface_SK.h"

#include <set>


namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{



class Converter
{
  public:
                  Converter();
    virtual       ~Converter();

    static void   convert(T::ContentInfo& source,ContentServer::Corba::CorbaContentInfo& target);
    static void   convert(const ContentServer::Corba::CorbaContentInfo& source,T::ContentInfo& target);

    static void   convert(T::ContentInfoList& source,ContentServer::Corba::CorbaContentInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaContentInfoList& source,T::ContentInfoList& target);

    static void   convert(T::FileInfo& source,ContentServer::Corba::CorbaFileInfo& target);
    static void   convert(const ContentServer::Corba::CorbaFileInfo& source,T::FileInfo& target);

    static void   convert(T::FileInfoList& source,ContentServer::Corba::CorbaFileInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaFileInfoList& source,T::FileInfoList& target);

    static void   convert(T::ServerInfo& source,ContentServer::Corba::CorbaServerInfo& target);
    static void   convert(const ContentServer::Corba::CorbaServerInfo& source,T::ServerInfo& target);

    static void   convert(T::ServerInfoList& source,ContentServer::Corba::CorbaServerInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaServerInfoList& source,T::ServerInfoList& target);

    static void   convert(T::ProducerInfo& source,ContentServer::Corba::CorbaProducerInfo& target);
    static void   convert(const ContentServer::Corba::CorbaProducerInfo& source,T::ProducerInfo& target);

    static void   convert(T::ProducerInfoList& source,ContentServer::Corba::CorbaProducerInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaProducerInfoList& source,T::ProducerInfoList& target);

    static void   convert(T::GenerationInfo& source,ContentServer::Corba::CorbaGenerationInfo& target);
    static void   convert(const ContentServer::Corba::CorbaGenerationInfo& source,T::GenerationInfo& target);

    static void   convert(T::GenerationInfoList& source,ContentServer::Corba::CorbaGenerationInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaGenerationInfoList& source,T::GenerationInfoList& target);

    static void   convert(T::EventInfo& source,ContentServer::Corba::CorbaEventInfo& target);
    static void   convert(const ContentServer::Corba::CorbaEventInfo& source,T::EventInfo& target);

    static void   convert(T::EventInfoList& source,ContentServer::Corba::CorbaEventInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaEventInfoList& source,T::EventInfoList& target);

    static void   convert(T::LevelInfo& source,ContentServer::Corba::CorbaLevelInfo& target);
    static void   convert(const ContentServer::Corba::CorbaLevelInfo& source,T::LevelInfo& target);

    static void   convert(T::LevelInfoList& source,ContentServer::Corba::CorbaLevelInfoList& target);
    static void   convert(const ContentServer::Corba::CorbaLevelInfoList& source,T::LevelInfoList& target);

    static void   convert(string_vec& source,ContentServer::Corba::CorbaStringList& target);
    static void   convert(const ContentServer::Corba::CorbaStringList& source,string_vec& target);

    static void   convert(std::set<std::string>& source,ContentServer::Corba::CorbaStringList& target);
    static void   convert(const ContentServer::Corba::CorbaStringList& source,std::set<std::string>& target);

    static void   convert(std::vector<uint>& source,ContentServer::Corba::CorbaULongList& target);
    static void   convert(const ContentServer::Corba::CorbaULongList& source,std::vector<uint>& target);

    static void   convert(std::set<uint>& source,ContentServer::Corba::CorbaULongList& target);
    static void   convert(const ContentServer::Corba::CorbaULongList& source,std::set<uint>& target);

    static void   convert(std::set<int>& source,ContentServer::Corba::CorbaLongList& target);
    static void   convert(const ContentServer::Corba::CorbaLongList& source,std::set<int>& target);

    static void   convert(T::FileAndContent& source,ContentServer::Corba::CorbaFileContent& target);
    static void   convert(const ContentServer::Corba::CorbaFileContent& source,T::FileAndContent& target);

    static void   convert(std::vector<T::FileAndContent>& source,ContentServer::Corba::CorbaFileContentList& target);
    static void   convert(const ContentServer::Corba::CorbaFileContentList& source,std::vector<T::FileAndContent>& target);

    static void   convert(T::ForecastTime& source,SmartMet::ContentServer::Corba::CorbaForecastTime& target);
    static void   convert(const SmartMet::ContentServer::Corba::CorbaForecastTime& source,T::ForecastTime& target);

    static void   convert(std::vector<T::ForecastTime>& source,SmartMet::ContentServer::Corba::CorbaForecastTimeList& target);
    static void   convert(const SmartMet::ContentServer::Corba::CorbaForecastTimeList& source,std::vector<T::ForecastTime>& target);
};



}
}
}

