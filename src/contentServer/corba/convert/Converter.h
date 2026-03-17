#pragma once

#include "../../definition/ContentInfo.h"
#include "../../definition/ContentInfoList.h"
#include "../../definition/FileInfo.h"
#include "../../definition/FileAndContent.h"
#include "../../definition/FileInfoList.h"
#include "../../definition/EventInfo.h"
#include "../../definition/EventInfoList.h"
#include "../../definition/ProducerInfo.h"
#include "../../definition/ProducerInfoList.h"
#include "../../definition/GenerationInfo.h"
#include "../../definition/GenerationInfoList.h"
#include "../../definition/GeometryInfo.h"
#include "../../definition/GeometryInfoList.h"
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

    static void   convert(T::ContentInfo& source,C::ContentInfo& target);
    static void   convert(const C::ContentInfo& source,T::ContentInfo& target);

    static void   convert(T::ContentInfoList& source,C::ContentInfoList& target);
    static void   convert(const C::ContentInfoList& source,T::ContentInfoList& target);

    static void   convert(T::FileInfo& source,C::FileInfo& target);
    static void   convert(const C::FileInfo& source,T::FileInfo& target);

    static void   convert(T::FileInfoList& source,C::FileInfoList& target);
    static void   convert(const C::FileInfoList& source,T::FileInfoList& target);

    static void   convert(T::ProducerInfo& source,C::ProducerInfo& target);
    static void   convert(const C::ProducerInfo& source,T::ProducerInfo& target);

    static void   convert(T::ProducerInfoList& source,C::ProducerInfoList& target);
    static void   convert(const C::ProducerInfoList& source,T::ProducerInfoList& target);

    static void   convert(T::GenerationInfo& source,C::GenerationInfo& target);
    static void   convert(const C::GenerationInfo& source,T::GenerationInfo& target);

    static void   convert(T::GenerationInfoList& source,C::GenerationInfoList& target);
    static void   convert(const C::GenerationInfoList& source,T::GenerationInfoList& target);

    static void   convert(T::GeometryInfo& source,C::GeometryInfo& target);
    static void   convert(const C::GeometryInfo& source,T::GeometryInfo& target);

    static void   convert(T::GeometryInfoList& source,C::GeometryInfoList& target);
    static void   convert(const C::GeometryInfoList& source,T::GeometryInfoList& target);

    static void   convert(T::EventInfo& source,C::EventInfo& target);
    static void   convert(const C::EventInfo& source,T::EventInfo& target);

    static void   convert(T::EventInfoList& source,C::EventInfoList& target);
    static void   convert(const C::EventInfoList& source,T::EventInfoList& target);

    static void   convert(T::LevelInfo& source,C::LevelInfo& target);
    static void   convert(const C::LevelInfo& source,T::LevelInfo& target);

    static void   convert(T::LevelInfoList& source,C::LevelInfoList& target);
    static void   convert(const C::LevelInfoList& source,T::LevelInfoList& target);

    static void   convert(string_vec& source,C::StringList& target);
    static void   convert(const C::StringList& source,string_vec& target);

    static void   convert(std::set<std::string>& source,C::StringList& target);
    static void   convert(const C::StringList& source,std::set<std::string>& target);

    static void   convert(std::set<std::string>& source,C::TimeStringList& target);
    static void   convert(const C::TimeStringList& source,std::set<std::string>& target);

    static void   convert(std::set<UInt64>& source,C::FileIdList& target);
    static void   convert(const C::FileIdList& source,std::set<UInt64>& target);

    static void   convert(std::vector<UInt64>& source,C::FileIdList& target);
    static void   convert(const C::FileIdList& source,std::vector<UInt64>& target);

    static void   convert(std::vector<uint>& source,C::UInt32List& target);
    static void   convert(const C::UInt32List& source,std::vector<uint>& target);

    static void   convert(std::set<UInt64>& source,C::GenerationIdList& target);
    static void   convert(const C::GenerationIdList& source,std::set<UInt64>& target);

    static void   convert(std::set<int>& source,C::GeometryIdList& target);
    static void   convert(const C::GeometryIdList& source,std::set<int>& target);

    static void   convert(std::set<int>& source,C::LevelList& target);
    static void   convert(const C::LevelList& source,std::set<int>& target);

    static void   convert(T::FileAndContent& source,C::FileContent& target);
    static void   convert(const C::FileContent& source,T::FileAndContent& target);

    static void   convert(std::vector<T::FileAndContent>& source,C::FileContentList& target);
    static void   convert(const C::FileContentList& source,std::vector<T::FileAndContent>& target);

    static void   convert(T::ForecastTime& source,SmartMet::C::ForecastTimeInfo& target);
    static void   convert(const SmartMet::C::ForecastTimeInfo& source,T::ForecastTime& target);

    static void   convert(std::vector<T::ForecastTime>& source,SmartMet::C::ForecastTimeInfoList& target);
    static void   convert(const SmartMet::C::ForecastTimeInfoList& source,std::vector<T::ForecastTime>& target);
};



}
}
}

