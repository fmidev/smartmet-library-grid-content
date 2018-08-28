#include "Alias.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{


Alias::Alias()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Alias::Alias(const Alias& alias)
{
  try
  {
    mName = alias.mName;
    mAliasString = alias.mAliasString;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Alias::~Alias()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}  // namespace QueryServer
}  // namespace SmartMet
