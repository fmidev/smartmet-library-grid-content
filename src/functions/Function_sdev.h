#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// Standard deviation

class Function_sdev : public Function
{
  public:
                      Function_sdev();
                      Function_sdev(const Function_sdev& function);
    virtual           ~Function_sdev();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
};


}  // namespace Functions
}  // namespace SmartMet
