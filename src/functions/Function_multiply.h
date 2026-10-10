#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{

// ====================================================================================
/*! \brief Multiplies every grid value by a constant scalar factor. */
// ====================================================================================

class Function_multiply : public Function
{
  public:
                      Function_multiply();
                      Function_multiply(double multiplyer);
                      Function_multiply(const Function_multiply& function);
    ~Function_multiply() override;

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;

    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;

  protected:

    double          mMultiplyer;  //!< Constant factor applied to each grid value.
};


}  // namespace Functions
}  // namespace SmartMet
