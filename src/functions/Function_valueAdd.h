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


class Function_valueAdd : public Function
{
  public:
                      Function_valueAdd(double valueToAdd);
                      Function_valueAdd(const Function_valueAdd& function);
    virtual           ~Function_valueAdd();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionType1(std::vector<float>& parameters);
    virtual double    executeFunctionType1(std::vector<double>& parameters);

    // Type 2: Takes and returns an vector
    virtual void      executeFunctionType2(std::vector<float>& inOutParameters);
    virtual void      executeFunctionType2(std::vector<double>& inOutParameters);
    virtual void      executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();

  protected:

    double            mValueToAdd;
};


}  // namespace Functions
}  // namespace SmartMet
