#include "ValueRecord.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/grid/Typedefs.h>

namespace SmartMet
{
namespace T
{



ValueRecord::ValueRecord()
{
  try
  {
    mFileId = 0;
    mMessageIndex = 0;
    mCoordinateType = T::CoordinateTypeValue::UNKNOWN;
    mAreaInterpolationMethod = T::AreaInterpolationMethod::Linear;
    mX = 0;
    mY = 0;
    mValue = 0;
    mResult = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ValueRecord::ValueRecord(ValueRecord& valueRecord)
{
  try
  {
    mFileId = valueRecord.mFileId;
    mMessageIndex = valueRecord.mMessageIndex;
    mCoordinateType = valueRecord.mCoordinateType;
    mAreaInterpolationMethod = valueRecord.mAreaInterpolationMethod;
    mX = valueRecord.mX;
    mY = valueRecord.mY;
    mValue = valueRecord.mValue;
    mResult = valueRecord.mResult;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




ValueRecord::~ValueRecord()
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





void ValueRecord::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ValueRecord\n";
    stream << space(level) << "- mFileId                  = " << mFileId << "\n";
    stream << space(level) << "- mMessageIndex            = " << mMessageIndex << "\n";
    stream << space(level) << "- mCoordinateType          = " << mCoordinateType << "\n";
    stream << space(level) << "- mAreaInterpolationMethod = " << mAreaInterpolationMethod << "\n";
    stream << space(level) << "- mX                       = " << mX << "\n";
    stream << space(level) << "- mY                       = " << mY << "\n";
    stream << space(level) << "- mValue                   = " << mValue << "\n";
    stream << space(level) << "- mResult                  = " << mResult << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}
}  // namespace SmartMet
