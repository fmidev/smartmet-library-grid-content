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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ForecastTime::~ForecastTime()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
      mGenerationId = atoi(partList[0].c_str());
      mGeometryId = atoi(partList[1].c_str());
      mForecastType = atoi(partList[2].c_str());
      mForecastNumber = atoi(partList[3].c_str());
      mForecastTime = partList[4];
      mModificationTime = partList[5];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}


}
}
