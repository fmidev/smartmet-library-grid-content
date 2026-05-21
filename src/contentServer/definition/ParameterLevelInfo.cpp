#include "ParameterLevelInfo.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for ParameterLevelInfo. */

ParameterLevelInfo::ParameterLevelInfo()
{
  try
  {
    mParameterKeyType = T::ParamKeyTypeValue::FMI_ID;
    mParameterLevelId = 0;
    mLevel = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for ParameterLevelInfo. */

ParameterLevelInfo::ParameterLevelInfo(const ParameterLevelInfo& levelInfo)
{
  try
  {
    mParameterKeyType = levelInfo.mParameterKeyType;
    mParameterKey = levelInfo.mParameterKey;
    mParameterLevelId = levelInfo.mParameterLevelId;
    mLevel = levelInfo.mLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Construct ParameterLevelInfo from the given parameter key and level fields. */

ParameterLevelInfo::ParameterLevelInfo(T::ParamKeyType parameterKeyType,
                   T::ParamId parameterKey,
                   T::ParamLevelId parameterLevelId,
                   T::ParamLevel level)
{
  try
  {
    mParameterKeyType = parameterKeyType;
    mParameterKey = parameterKey;
    mParameterLevelId = parameterLevelId;
    mLevel = level;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for ParameterLevelInfo. */

ParameterLevelInfo::~ParameterLevelInfo()
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





/*! \brief Copy assignment operator for ParameterLevelInfo. */

ParameterLevelInfo& ParameterLevelInfo::operator=(const ParameterLevelInfo& levelInfo)
{
  try
  {
    if (&levelInfo == this)
      return *this;

    mParameterKeyType = levelInfo.mParameterKeyType;
    mParameterKey = levelInfo.mParameterKey;
    mParameterLevelId = levelInfo.mParameterLevelId;
    mLevel = levelInfo.mLevel;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return a deep copy of this ParameterLevelInfo. */

ParameterLevelInfo* ParameterLevelInfo::duplicate()
{
  try
  {
    return new ParameterLevelInfo(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Print the record contents to the given stream. */

void ParameterLevelInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterLevelInfo\n";
    stream << space(level) << "- mParameterKeyType     = " << C_INT(mParameterKeyType) << "\n";
    stream << space(level) << "- mParameterKey         = " << mParameterKey << "\n";
    stream << space(level) << "- mParameterLevelId     = " << C_INT(mParameterLevelId) << "\n";
    stream << space(level) << "- mLevel                = " << mLevel << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
