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
    mId = contentDefinition.mId;
    mVirtualParameter = contentDefinition.mVirtualParameter;
    mSourceParameters = contentDefinition.mSourceParameters;
    mSourceParameterNames = contentDefinition.mSourceParameterNames;
    mFunctionName = contentDefinition.mFunctionName;
    mFunctionCallMethod = contentDefinition.mFunctionCallMethod;
    mFunctionParameters = contentDefinition.mFunctionParameters;

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
    stream << space(level) << "- mId                 = " << mId << "\n";
    stream << space(level) << "- mVirtualParameter   = \n";
    mVirtualParameter.print(stream,level+2,optionFlags);
    stream << space(level) << "- mSourceParameters   = ";

    for (auto it = mSourceParameters.begin(); it != mSourceParameters.end(); ++it)
      it->print(stream,level+2,optionFlags);

    stream << space(level) << "- mFunctionName       = " << mFunctionName << "\n";
    stream << space(level) << "- mFunctionCallMethod = " << mFunctionCallMethod << "\n";
    stream << space(level) << "- mFunctionParameters = \n";
    for (auto it = mFunctionParameters.begin(); it != mFunctionParameters.end(); ++it)
      stream << space(level+2) << "* " << *it << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

}  // namespace DataServer
}  // namespace SmartMet
