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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





VirtualContentDefinition::VirtualContentDefinition(const VirtualContentDefinition& contentDefinition)
{
  try
  {
    mTargetParameter = contentDefinition.mTargetParameter;
    mSourceParameters = contentDefinition.mSourceParameters;
    mFunctionName = contentDefinition.mFunctionName;
    mFunctionCallMethod = contentDefinition.mFunctionCallMethod;
    mProducerNameList = contentDefinition.mProducerNameList;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





VirtualContentDefinition::~VirtualContentDefinition()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void VirtualContentDefinition::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "VirtualContentDefinition\n";
    stream << space(level) << "- mTargetParameter    = " << mTargetParameter << "\n";
    stream << space(level) << "- mSourceParameters   = ";

    for (auto it = mSourceParameters.begin(); it != mSourceParameters.end(); ++it)
      stream << *it << " ";

    stream << "\n";

    stream << space(level) << "- mFunctionName       = " << mFunctionName << "\n";
    stream << space(level) << "- mFunctionCallMethod = " << mFunctionCallMethod << "\n";
    stream << space(level) << "- mProducerNameList   = ";

    for (auto it = mProducerNameList.begin(); it != mProducerNameList.end(); ++it)
      stream << *it << " ";

    stream << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

}  // namespace DataServer
}  // namespace SmartMet
