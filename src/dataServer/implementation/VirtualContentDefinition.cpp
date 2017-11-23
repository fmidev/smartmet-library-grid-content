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
    mProducers = contentDefinition.mProducers;
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

    stream << space(level) << "- mFunctionName = " << mFunctionName << "\n";
    stream << space(level) << "- mProducers          = " << mProducers << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}

}  // namespace DataServer
}  // namespace SmartMet
