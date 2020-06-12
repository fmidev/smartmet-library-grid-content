#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// The function multiplies each input parameter by the value 'multiplyer'.

class Function_multiply : public Function
{
  public:
                      Function_multiply();
                      Function_multiply(double multiplyer);
                      Function_multiply(const Function_multiply& function);
    virtual           ~Function_multiply();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();

  protected:

    double          mMultiplyer;
};


}  // namespace Functions
}  // namespace SmartMet
