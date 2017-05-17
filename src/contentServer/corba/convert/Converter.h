#pragma once

#include "contentServer/definition/ContentInfo.h"
#include "contentServer/definition/ContentInfoList.h"
#include "contentServer/definition/FileInfo.h"
#include "contentServer/definition/FileInfoList.h"
#include "contentServer/definition/EventInfo.h"
#include "contentServer/definition/EventInfoList.h"
#include "contentServer/definition/ServerInfo.h"
#include "contentServer/definition/ServerInfoList.h"
#include "contentServer/definition/ProducerInfo.h"
#include "contentServer/definition/ProducerInfoList.h"
#include "contentServer/definition/GenerationInfo.h"
#include "contentServer/definition/GenerationInfoList.h"

#include "contentServer/corba/stubs/ContentServer_serviceInterface_SK.h"



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
    static void   convert(ContentServer::Corba::CorbaFileInfo& source,T::FileInfo& target);

    static void   convert(T::FileInfoList& source,ContentServer::Corba::CorbaFileInfoList& target);
    static void   convert(ContentServer::Corba::CorbaFileInfoList& source,T::FileInfoList& target);

    static void   convert(T::ServerInfo& source,ContentServer::Corba::CorbaServerInfo& target);
    static void   convert(ContentServer::Corba::CorbaServerInfo& source,T::ServerInfo& target);

    static void   convert(T::ServerInfoList& source,ContentServer::Corba::CorbaServerInfoList& target);
    static void   convert(ContentServer::Corba::CorbaServerInfoList& source,T::ServerInfoList& target);

    static void   convert(T::ProducerInfo& source,ContentServer::Corba::CorbaProducerInfo& target);
    static void   convert(ContentServer::Corba::CorbaProducerInfo& source,T::ProducerInfo& target);

    static void   convert(T::ProducerInfoList& source,ContentServer::Corba::CorbaProducerInfoList& target);
    static void   convert(ContentServer::Corba::CorbaProducerInfoList& source,T::ProducerInfoList& target);

    static void   convert(T::GenerationInfo& source,ContentServer::Corba::CorbaGenerationInfo& target);
    static void   convert(ContentServer::Corba::CorbaGenerationInfo& source,T::GenerationInfo& target);

    static void   convert(T::GenerationInfoList& source,ContentServer::Corba::CorbaGenerationInfoList& target);
    static void   convert(ContentServer::Corba::CorbaGenerationInfoList& source,T::GenerationInfoList& target);

    static void   convert(T::EventInfo& source,ContentServer::Corba::CorbaEventInfo& target);
    static void   convert(ContentServer::Corba::CorbaEventInfo& source,T::EventInfo& target);

    static void   convert(T::EventInfoList& source,ContentServer::Corba::CorbaEventInfoList& target);
    static void   convert(ContentServer::Corba::CorbaEventInfoList& source,T::EventInfoList& target);
};



}
}
}

