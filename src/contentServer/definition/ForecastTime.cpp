#include "ForecastTime.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{


ForecastTime::ForecastTime()
{
  try
  {
    mGenerationId = 0;
    mGeometryId = 0;
    mForecastType = 0;
    mForecastNumber = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ForecastTime::ForecastTime(const ForecastTime& forecastTime)
{
  try
  {
    mGenerationId = forecastTime.mGenerationId;
    mGeometryId = forecastTime.mGeometryId;
    mForecastType = forecastTime.mForecastType;
    mForecastNumber = forecastTime.mForecastNumber;
    mForecastTime = forecastTime.mForecastTime;
    mModificationTime = forecastTime.mModificationTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ForecastTime::ForecastTime(const char *csv)
{
  try
  {
    mGenerationId = 0;
    mGeometryId = 0;
    mForecastType = 0;
    mForecastNumber = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ForecastTime::~ForecastTime()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string ForecastTime::getCsv()
{
  try
  {
    char buf[200];
    sprintf(buf,"%d;%d;%d;%d;%s;%s",
        mGenerationId,
        mGeometryId,
        mForecastType,
        mForecastNumber,
        mForecastTime.c_str(),
        mModificationTime.c_str());

    return std::string(buf);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ForecastTime::setCsv(const char *csv)
{
  try
  {
    std::vector<std::string> partList;
    splitString(csv,';',partList);

    if (partList.size() >= 6)
    {
      mGenerationId = toInt64(partList[0].c_str());
      mGeometryId = toInt64(partList[1].c_str());
      mForecastType = toInt64(partList[2].c_str());
      mForecastNumber = toInt64(partList[3].c_str());
      mForecastTime = partList[4];
      mModificationTime = partList[5];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ForecastTime::setCsv(std::string csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ForecastTime::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ForecastTime\n";
    stream << space(level) << "- mGenerationId      = " << mGenerationId << "\n";
    stream << space(level) << "- mGeometryId        = " << mGeometryId << "\n";
    stream << space(level) << "- mForecastType      = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber    = " << mForecastNumber << "\n";
    stream << space(level) << "- mForecastTime      = " << mForecastTime << "\n";
    stream << space(level) << "- mModificationTime  = " << mModificationTime << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}
