#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// The function adds the value 'valueToAdd" into each input parameter. For
// example if the input parameters represent temperature in Kelvins, then
// we can easily create a conversion functio "Kelvins => Celsius" by setting
// valueToAdd = -273.15.


class Function_add : public Function
{
  public:
                      Function_add();
                      Function_add(double valueToAdd);
                      Function_add(const Function_add& function);
    virtual           ~Function_add();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();

  protected:

    double            mValueToAdd;
};


}  // namespace Functions
}  // namespace SmartMet
