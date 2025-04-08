#include "UnitConversion.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


UnitConversion::UnitConversion()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




UnitConversion::~UnitConversion()
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





void UnitConversion::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "UnitConversion\n";
    stream << space(level) << "- mSourceUnit                 = " << mSourceUnit << "\n";
    stream << space(level) << "- mTargetUnit                 = " << mTargetUnit << "\n";
    stream << space(level) << "- mConversionFunction         = " << mConversionFunction << "\n";
    stream << space(level) << "- mReverseConversionFunction  = " << mReverseConversionFunction << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
