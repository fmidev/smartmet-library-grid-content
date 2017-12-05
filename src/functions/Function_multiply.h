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

    double          mMultiplyer;
};


}  // namespace Functions
}  // namespace SmartMet
