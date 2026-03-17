#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>


namespace SmartMet
{
namespace T
{


class ForecastTime
{
  public:
                       ForecastTime();
                       ForecastTime(const ForecastTime& forecastTime);
                       ForecastTime(const char *csv);
     virtual           ~ForecastTime();

     std::string       getCsv();
     void              setCsv(const char *csv);
     void              setCsv(const std::string& csv);

     void              print(std::ostream& stream,uint level,uint optionFlags);

     T::StorageId      mStorageId;
     T::SourceId       mSourceId;
     T::GenerationId   mGenerationId;
     T::GeometryId     mGeometryId;
     T::ForecastType   mForecastType;
     T::ForecastNumber mForecastNumber;
     std::string       mForecastTime;
     std::string       mModificationTime;
};



}
}
