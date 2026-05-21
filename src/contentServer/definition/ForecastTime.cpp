#include "ForecastTime.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for ForecastTime. */

ForecastTime::ForecastTime()
{
  try
  {
    mSourceId = 0;
    mGenerationId = 0;
    mGeometryId = 0;
    mForecastType = 0;
    mForecastNumber = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for ForecastTime. */

ForecastTime::ForecastTime(const ForecastTime& forecastTime)
{
  try
  {
    mSourceId = forecastTime.mSourceId;
    mGenerationId = forecastTime.mGenerationId;
    mGeometryId = forecastTime.mGeometryId;
    mForecastType = forecastTime.mForecastType;
    mForecastNumber = forecastTime.mForecastNumber;
    mForecastTime = forecastTime.mForecastTime;
    mModificationTime = forecastTime.mModificationTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Construct ForecastTime from a CSV-formatted string. */

ForecastTime::ForecastTime(const char *csv)
{
  try
  {
    mSourceId = 0;
    mGenerationId = 0;
    mGeometryId = 0;
    mForecastType = 0;
    mForecastNumber = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for ForecastTime. */

ForecastTime::~ForecastTime()
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





/*! \brief Return this record formatted as a CSV row. */

std::string ForecastTime::getCsv()
{
  try
  {
    char buf[1000];
    snprintf(buf,sizeof(buf),"%u;%lu;%d;%d;%d;%s;%s",
        mSourceId,
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row C string. */

void ForecastTime::setCsv(const char *csv)
{
  try
  {
    std::vector<std::string> partList;
    splitString(csv,';',partList);

    if (partList.size() >= 7)
    {
      mSourceId = toUInt32(partList[0].c_str());
      mGenerationId = toUInt64(partList[1].c_str());
      mGeometryId = toInt32(partList[2].c_str());
      mForecastType = toInt16(partList[3].c_str());
      mForecastNumber = toInt16(partList[4].c_str());
      mForecastTime = partList[5];
      mModificationTime = partList[6];
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row std::string. */

void ForecastTime::setCsv(const std::string& csv)
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





/*! \brief Print the record contents to the given stream. */

void ForecastTime::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ForecastTime\n";
    stream << space(level) << "- mSourceId          = " << mSourceId << "\n";
    stream << space(level) << "- mGenerationId      = " << mGenerationId << "\n";
    stream << space(level) << "- mGeometryId        = " << mGeometryId << "\n";
    stream << space(level) << "- mForecastType      = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber    = " << mForecastNumber << "\n";
    stream << space(level) << "- mForecastTime      = " << mForecastTime << "\n";
    stream << space(level) << "- mModificationTime  = " << mModificationTime << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
