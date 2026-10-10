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
    ~Function_sequence() override;

    virtual void      addFunction(Function *function);

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;


    Function* duplicate() override;

  protected:

    FunctionPtr_vec mFunctionList;  //!< Ordered list of functions to execute in sequence.
};


}  // namespace Functions
}  // namespace SmartMet
