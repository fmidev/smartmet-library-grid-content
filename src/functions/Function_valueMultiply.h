#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// The function multiplies each input parameter by the value 'multiplyer'.

class Function_valueMultiply : public Function
{
  public:
                      Function_valueMultiply(double multiplyer);
                      Function_valueMultiply(const Function_valueMultiply& function);
    virtual           ~Function_valueMultiply();

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

    double          mMultiplyer;
};


}  // namespace Functions
}  // namespace SmartMet
