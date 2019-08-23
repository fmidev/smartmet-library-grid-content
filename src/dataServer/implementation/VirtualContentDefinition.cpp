#include "VirtualContentDefinition.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace DataServer
{


VirtualContentDefinition::VirtualContentDefinition()
{
  try
  {
    mFunctionCallMethod = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinition::VirtualContentDefinition(const VirtualContentDefinition& contentDefinition)
{
  try
  {
    mVirtualParameter = contentDefinition.mVirtualParameter;
    mSourceParameters = contentDefinition.mSourceParameters;
    mFunctionName = contentDefinition.mFunctionName;
    mFunctionCallMethod = contentDefinition.mFunctionCallMethod;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinition::~VirtualContentDefinition()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void VirtualContentDefinition::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "VirtualContentDefinition\n";
    stream << space(level) << "- mVirtualParameter   = \n";
    mVirtualParameter.print(stream,level+2,optionFlags);
    stream << space(level) << "- mSourceParameters   = ";

    for (auto it = mSourceParameters.begin(); it != mSourceParameters.end(); ++it)
      it->print(stream,level+2,optionFlags);

    stream << space(level) << "- mFunctionName       = " << mFunctionName << "\n";
    stream << space(level) << "- mFunctionCallMethod = " << mFunctionCallMethod << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

}  // namespace DataServer
}  // namespace SmartMet
