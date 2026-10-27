#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Computes the element-wise difference between two consecutive time-step grids. */
// ====================================================================================

class Function_diff : public Function
{
  public:
                      Function_diff();
                      Function_diff(const Function_diff& function);
    ~Function_diff() override;
    Function_diff& operator=(const Function_diff&) = default;

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
