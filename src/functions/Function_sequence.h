#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Chains a sequence of functions so each function's output becomes the next function's input. */
// ====================================================================================

class Function_sequence : public Function
{
  public:
                      Function_sequence();
                      Function_sequence(const Function_sequence& function);
    virtual           ~Function_sequence();

    virtual void      addFunction(Function *function);

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);


    virtual Function* duplicate();

  protected:

    FunctionPtr_vec mFunctionList;  //!< Ordered list of functions to execute in sequence.
};


}  // namespace Functions
}  // namespace SmartMet
