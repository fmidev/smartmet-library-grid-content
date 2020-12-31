#include "ParameterDef.h"
#include <grid-files/common/GeneralFunctions.h>
#include <boost/functional/hash.hpp>


namespace SmartMet
{
namespace DataServer
{



ParameterDef::ParameterDef()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*
ParameterDef::ParameterDef(const ParameterDef& def)
{
  try
  {
    mParameterName = def.mParameterName;
    mProducerName = def.mProducerName;
    mGeometryId = def.mGeometryId;
    mLevelId = def.mLevelId;
    mLevel = def.mLevel;
    mForecastType = def.mForecastType;
    mForecastNumber = def.mForecastNumber;
    mAreaInterpolationMethod = def.mAreaInterpolationMethod;
    mTimeInterpolationMethod = def.mTimeInterpolationMethod;
    mLevelInterpolationMethod = def.mLevelInterpolationMethod;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




ParameterDef::ParameterDef(const std::string& paramStr)
{
  try
  {
    std::vector<std::string> fields;

    splitString(paramStr,':',fields);
    int c = fields.size();

    if (c > 0)
      mParameterName = fields[0];

    if (c > 1)
      mProducerName = fields[1];

    if (c > 2)
      mGeometryId = fields[2];

    if (c > 3)
      mLevelId = fields[3];

    if (c > 4)
      mLevel = fields[4];

    if (c > 5)
      mForecastType = fields[5];

    if (c > 6)
      mForecastNumber = fields[6];

    if (c > 7)
      mAreaInterpolationMethod = fields[7];

    if (c > 8)
      mTimeInterpolationMethod = fields[8];

    if (c > 9)
      mLevelInterpolationMethod = fields[9];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






std::string ParameterDef::getName()
{
  try
  {
    std::string name = mParameterName + ":" + mProducerName  + ":" + mGeometryId  + ":" + mLevelId  + ":" + mLevel;

    if (mForecastType > " ")
      name = name + ":" + mForecastType;

    if (mForecastNumber > " ")
      name = name + ":" + mForecastNumber;

    return name;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void ParameterDef::set(const std::string& paramStr)
{
  try
  {
    mParameterName = "";
    mProducerName = "";
    mGeometryId = "";
    mLevelId = "";
    mLevel = "";
    mForecastType = "";
    mForecastNumber = "";
    mAreaInterpolationMethod = "";
    mTimeInterpolationMethod = "";
    mLevelInterpolationMethod = "";

    std::vector<std::string> fields;

    splitString(paramStr,':',fields);
    int c = fields.size();

    if (c > 0)
      mParameterName = fields[0];

    if (c > 1)
      mProducerName = fields[1];

    if (c > 2)
      mGeometryId = fields[2];

    if (c > 3)
      mLevelId = fields[3];

    if (c > 4)
      mLevel = fields[4];

    if (c > 5)
      mForecastType = fields[5];

    if (c > 6)
      mForecastNumber = fields[6];

    if (c > 7)
      mAreaInterpolationMethod = fields[7];

    if (c > 8)
      mTimeInterpolationMethod = fields[8];

    if (c > 9)
      mLevelInterpolationMethod = fields[9];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





ParameterDef::~ParameterDef()
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






void ParameterDef::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterDef\n";
    stream << space(level) << "- mParameterName             = " << mParameterName << "\n";
    stream << space(level) << "- mProducerName              = " << mProducerName << "\n";
    stream << space(level) << "- mGeometryId                = " << mGeometryId << "\n";
    stream << space(level) << "- mLevelId                   = " << mLevelId << "\n";
    stream << space(level) << "- mLevel                     = " << mLevel << "\n";
    stream << space(level) << "- mForecastType              = " << mForecastType << "\n";
    stream << space(level) << "- mForecastNumber            = " << mForecastNumber << "\n";
    stream << space(level) << "- mAreaInterpolationMethod   = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mTimeInterpolationMethod   = " << mTimeInterpolationMethod << "\n";
    stream << space(level) << "- mLevelInterpolationMethod  = " << mLevelInterpolationMethod << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
