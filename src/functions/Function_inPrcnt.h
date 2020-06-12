#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


class Function_inPrcnt : public Function
{
  public:
                      Function_inPrcnt();
                      Function_inPrcnt(const Function_inPrcnt& function);
    virtual           ~Function_inPrcnt();

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
};


}  // namespace Functions
}  // namespace SmartMet
