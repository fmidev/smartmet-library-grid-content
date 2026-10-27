#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Tests whether each grid value is greater than or equal to a threshold, returning 1 or 0. */
// ====================================================================================

class Function_gte : public Function
{
  public:
                      Function_gte();
                      Function_gte(const Function_gte& function);
    ~Function_gte() override;
    Function_gte& operator=(const Function_gte&) = default;

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;

    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;
};


}  // namespace Functions
}  // namespace SmartMet
