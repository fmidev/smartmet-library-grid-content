#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// This function is actually a sequence of functions. If executes each function
// in the list so that the output of the executed function will be the input
// of the next executed function. In this way can execute more complex formulas.

// It is good to realize that this kind of execution is not the fastest way
// to count formulas i.e. it is usually better to create own function class for
// complex formulas rather than call several smaller functions in sequence.


class Function_sequence : public Function
{
  public:
                      Function_sequence();
                      Function_sequence(const Function_sequence& function);
    virtual           ~Function_sequence();

    virtual void      addFunction(Function *function);

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

    FunctionPtr_vec mFunctionList;
};


}  // namespace Functions
}  // namespace SmartMet
