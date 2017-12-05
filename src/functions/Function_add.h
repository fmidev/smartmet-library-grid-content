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

    // Type 2: Takes and returns an vector
    virtual void      executeFunctionCall2(uint columns,uint rows,std::vector<float>& inOutParameters);
    virtual void      executeFunctionCall2(uint columns,uint rows,std::vector<double>& inOutParameters);
    virtual void      executeFunctionCall2(uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall2(uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters);

    // Type 3: Takes two vectors and returns one vector
    virtual void      executeFunctionCall3(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters);
    virtual void      executeFunctionCall3(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters);

    virtual Function* duplicate();

  protected:

    double            mValueToAdd;
};


}  // namespace Functions
}  // namespace SmartMet
