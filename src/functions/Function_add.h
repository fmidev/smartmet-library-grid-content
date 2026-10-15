#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// ====================================================================================
/*! \brief Adds a constant scalar offset to every grid value (e.g. Kelvin to Celsius). */
// ====================================================================================

class Function_add : public Function
{
  public:
                      Function_add();
                      Function_add(double valueToAdd);
                      Function_add(const Function_add& function);
    ~Function_add() override;
    Function_add& operator=(const Function_add&) = default;

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;

    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;

  protected:

    double            mValueToAdd;  //!< Scalar offset added to each grid value.
};


}  // namespace Functions
}  // namespace SmartMet
