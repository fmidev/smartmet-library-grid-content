#pragma once

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
     virtual          ~ForecastTime();

     std::string      getCsv();
     void             setCsv(const char *csv);
     void             setCsv(std::string csv);

     void             print(std::ostream& stream,uint level,uint optionFlags);

     uint             mGenerationId;
     uint             mGeometryId;
     short            mForecastType;
     short            mForecastNumber;
     std::string      mForecastTime;
     std::string      mModificationTime;
};



}
}
