#include "Alias.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


/*! \brief Query Server: Constructor. */

Alias::Alias()
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
Alias::Alias(const Alias& alias)
{
  try
  {
    mName = alias.mName;
    mAliasString = alias.mAliasString;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/




/*! \brief Query Server: Destructor. */

Alias::~Alias()
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

void Alias::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Alias\n";
    stream << space(level) << "- mName         = " << mName << "\n";
    stream << space(level) << "- mAliasString  = " << mAliasString << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
