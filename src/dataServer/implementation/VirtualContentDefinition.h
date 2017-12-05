#pragma once

#include <grid-files/grid/Typedefs.h>


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

    std::string           mTargetParameter;
    string_vec            mSourceParameters;
    std::string           mFunctionName;
    uint                  mFunctionCallMethod;
    std::set<std::string> mProducerNameList;
};



typedef std::vector<VirtualContentDefinition> VirtualContentDefinition_vec;


}  // namespace DataServer
}  // namespace SmartMet
