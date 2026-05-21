#include "LevelInfo.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for LevelInfo. */

LevelInfo::LevelInfo()
{
  try
  {
    mProducerId = 0;
    mFmiParameterLevelId = 0;
    mParameterLevel = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for LevelInfo. */

LevelInfo::LevelInfo(const LevelInfo& levelInfo)
{
  try
  {
    mProducerId = levelInfo.mProducerId;
    mFmiParameterName = levelInfo.mFmiParameterName;
    mFmiParameterLevelId = levelInfo.mFmiParameterLevelId;
    mParameterLevel = levelInfo.mParameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Construct LevelInfo from the given producer, parameter name, level id, and level. */

LevelInfo::LevelInfo(T::ProducerId producerId,std::string fmiParameterName,T::ParamLevelId fmiParameterLevelId,T::ParamLevel parameterLevel)
{
  try
  {
    mProducerId = producerId;
    mFmiParameterName = fmiParameterName;
    mFmiParameterLevelId = fmiParameterLevelId;
    mParameterLevel = parameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for LevelInfo. */

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





/*! \brief Copy assignment operator for LevelInfo. */

LevelInfo& LevelInfo::operator=(const LevelInfo& levelInfo)
{
  try
  {
    if (&levelInfo == this)
      return *this;

    mProducerId = levelInfo.mProducerId;
    mFmiParameterName = levelInfo.mFmiParameterName;
    mFmiParameterLevelId = levelInfo.mFmiParameterLevelId;
    mParameterLevel = levelInfo.mParameterLevel;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Return this record formatted as a CSV row. */

std::string LevelInfo::getCsv()
{
  try
  {
    char st[1000];
    snprintf(st,sizeof(st),"%u;%u;;;%d",
        mProducerId,
        mFmiParameterLevelId,
        mParameterLevel);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the CSV header row describing the fields. */

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





/*! \brief Populate this record from a CSV row C string. */

void LevelInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    std::string tmp(csv);

    uint c = 0;
    field[0] = tmp.data();
    char *p = tmp.data();
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
      mParameterLevel = toInt32(field[5]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Populate this record from a CSV row std::string. */

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





/*! \brief Return a deep copy of this LevelInfo. */

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




/*! \brief Print the record contents to the given stream. */

void LevelInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LevelInfo\n";
    stream << space(level) << "- mProducerId            = " << mProducerId << "\n";
    stream << space(level) << "- mFmiParameterName      = " << mFmiParameterName << "\n";
    stream << space(level) << "- mFmiParameterLevelId   = " << C_INT(mFmiParameterLevelId) << "\n";
    stream << space(level) << "- mParameterLevel        = " << mParameterLevel << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
