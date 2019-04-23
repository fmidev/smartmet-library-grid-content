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
    mGeometryId = -1;
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
    mProducerName = contentDefinition.mProducerName;
    mGeometryId = contentDefinition.mGeometryId;
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
    stream << space(level) << "- mVirtualParameter   = " << mVirtualParameter << "\n";
    stream << space(level) << "- mSourceParameters   = ";

    for (auto it = mSourceParameters.begin(); it != mSourceParameters.end(); ++it)
      stream << *it << " ";

    stream << "\n";

    stream << space(level) << "- mFunctionName       = " << mFunctionName << "\n";
    stream << space(level) << "- mFunctionCallMethod = " << mFunctionCallMethod << "\n";
    stream << space(level) << "- mProducerName       = " << mProducerName << "\n";
    stream << space(level) << "- mGeometryId         = " << mGeometryId << "\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}

}  // namespace DataServer
}  // namespace SmartMet
