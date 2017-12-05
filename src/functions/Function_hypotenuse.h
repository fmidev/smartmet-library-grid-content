#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


class Function_hypotenuse : public Function
{
  public:
                      Function_hypotenuse();
                      Function_hypotenuse(const Function_hypotenuse& function);
    virtual           ~Function_hypotenuse();

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
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);
};



}  // namespace Functions
}  // namespace SmartMet
