#include "LevelInfo.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


LevelInfo::LevelInfo()
{
  try
  {
    mProducerId = 0;
    mFmiParameterLevelId = 0;
    mGrib1ParameterLevelId = 0;
    mGrib2ParameterLevelId = 0;
    mParameterLevel = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





LevelInfo::LevelInfo(const LevelInfo& levelInfo)
{
  try
  {
    mProducerId = levelInfo.mProducerId;
    mFmiParameterName = levelInfo.mFmiParameterName;
    mFmiParameterLevelId = levelInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = levelInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = levelInfo.mGrib2ParameterLevelId;
    mParameterLevel = levelInfo.mParameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





LevelInfo::LevelInfo(uint producerId,std::string fmiParameterName,T::ParamLevelId fmiParameterLevelId,T::ParamLevelId grib1ParameterLevelId,T::ParamLevelId grib2ParameterLevelId,T::ParamLevel parameterLevel)
{
  try
  {
    mProducerId = producerId;
    mFmiParameterName = fmiParameterName;
    mFmiParameterLevelId = fmiParameterLevelId;
    mGrib1ParameterLevelId = grib1ParameterLevelId;
    mGrib2ParameterLevelId = grib2ParameterLevelId;
    mParameterLevel = parameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





LevelInfo::~LevelInfo()
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





LevelInfo& LevelInfo::operator=(const LevelInfo& levelInfo)
{
  try
  {
    if (&levelInfo == this)
      return *this;

    mProducerId = levelInfo.mProducerId;
    mFmiParameterName = levelInfo.mFmiParameterName;
    mFmiParameterLevelId = levelInfo.mFmiParameterLevelId;
    mGrib1ParameterLevelId = levelInfo.mGrib1ParameterLevelId;
    mGrib2ParameterLevelId = levelInfo.mGrib2ParameterLevelId;
    mParameterLevel = levelInfo.mParameterLevel;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::string LevelInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%u;%u;%d",
        mProducerId,
        mFmiParameterLevelId,
        mGrib1ParameterLevelId,
        mGrib2ParameterLevelId,
        mParameterLevel);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string LevelInfo::getCsvHeader()
{
  try
  {
    std::string header = "producerId;fmiParameterName;fmiParameterLevelId;grib1ParameterLevelId;grib2ParameterLevelId;parameterLevel";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void LevelInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    char st[1000];
    strcpy(st,csv);

    uint c = 0;
    field[0] = st;
    char *p = st;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == ';'  ||  *p == '\n')
      {
        *p = '\0';
        p++;
        c++;
        field[c] = p;
      }
      else
      {
        p++;
      }
    }

    if (c >= 6)
    {
      mProducerId = toUInt32(field[0]);
      mFmiParameterName = field[1];
      mFmiParameterLevelId = toUInt8(field[2]);
      mGrib1ParameterLevelId = toUInt8(field[3]);
      mGrib2ParameterLevelId = toUInt8(field[4]);
      mParameterLevel = toInt32(field[5]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void LevelInfo::setCsv(const std::string& csv)
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





LevelInfo* LevelInfo::duplicate()
{
  try
  {
    return new LevelInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void LevelInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LevelInfo\n";
    stream << space(level) << "- mProducerId            = " << mProducerId << "\n";
    stream << space(level) << "- mFmiParameterName      = " << mFmiParameterName << "\n";
    stream << space(level) << "- mFmiParameterLevelId   = " << C_INT(mFmiParameterLevelId) << "\n";
    stream << space(level) << "- mGrib1ParameterLevelId = " << C_INT(mGrib1ParameterLevelId) << "\n";
    stream << space(level) << "- mGrib2ParameterLevelId = " << C_INT(mGrib2ParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel        = " << mParameterLevel << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
