#include "ValueRecord.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"
#include "grid-files/grid/Typedefs.h"

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
    mCoordinateType = T::CoordinateType::UNKNOWN;
    mInterpolationMethod = T::InterpolationMethod::Linear;
    mX = 0;
    mY = 0;
    mValue = 0;
    mResult = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ValueRecord::ValueRecord(ValueRecord& valueRecord)
{
  try
  {
    mFileId = valueRecord.mFileId;
    mMessageIndex = valueRecord.mMessageIndex;
    mCoordinateType = valueRecord.mCoordinateType;
    mInterpolationMethod = valueRecord.mInterpolationMethod;
    mX = valueRecord.mX;
    mY = valueRecord.mY;
    mValue = valueRecord.mValue;
    mResult = valueRecord.mResult;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




ValueRecord::~ValueRecord()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ValueRecord::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ValueRecord\n";
    stream << space(level) << "- mFileId              = " << mFileId << "\n";
    stream << space(level) << "- mMessageIndex        = " << mMessageIndex << "\n";
    stream << space(level) << "- mCoordinateType      = " << (int)mCoordinateType << "\n";
    stream << space(level) << "- mInterpolationMethod = " << (int)mInterpolationMethod << "\n";
    stream << space(level) << "- mX                   = " << mX << "\n";
    stream << space(level) << "- mY                   = " << mY << "\n";
    stream << space(level) << "- mValue               = " << mValue << "\n";
    stream << space(level) << "- mResult              = " << mResult << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}  // namespace SmartMet
