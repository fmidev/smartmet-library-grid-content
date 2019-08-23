#pragma once

#include <grid-files/grid/Typedefs.h>
#include "ParameterDef.h"

namespace SmartMet
{
namespace DataServer
{


class VirtualContentDefinition
{
  public:
                          VirtualContentDefinition();
                          VirtualContentDefinition(const VirtualContentDefinition& contentDefinition);
    virtual               ~VirtualContentDefinition();

    void                  print(std::ostream& stream,uint level,uint optionFlags);

    ParameterDef          mVirtualParameter;
    ParameterDef_vec      mSourceParameters;
    std::string           mFunctionName;
    uint                  mFunctionCallMethod;
};



typedef std::vector<VirtualContentDefinition> VirtualContentDefinition_vec;


}  // namespace DataServer
}  // namespace SmartMet
