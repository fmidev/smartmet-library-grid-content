#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Computes sqrt(x^2 + y^2) element-wise from two input grids. */
// ====================================================================================

class Function_hypotenuse : public Function
{
  public:
                      Function_hypotenuse();
                      Function_hypotenuse(const Function_hypotenuse& function);
    virtual           ~Function_hypotenuse();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);
};



}  // namespace Functions
}  // namespace SmartMet
