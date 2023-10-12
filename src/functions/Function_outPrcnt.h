#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


class Function_outPrcnt : public Function
{
  public:
                      Function_outPrcnt();
                      Function_outPrcnt(const Function_outPrcnt& function);
    virtual           ~Function_outPrcnt();

    // Type 1: First and second param define the range (min,max) and the rest belongs to data.
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
};


}  // namespace Functions
}  // namespace SmartMet
