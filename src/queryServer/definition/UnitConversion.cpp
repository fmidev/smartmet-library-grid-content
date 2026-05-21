#include "UnitConversion.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


/*! \brief Query Server: Constructor. */

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




/*! \brief Query Server: Destructor. */

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





/*! \brief Query Server: Print. */

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
