#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Clamps every grid value to a specified [min, max] range. */
// ====================================================================================

class Function_limit : public Function
{
  public:
                      Function_limit();
                      Function_limit(const Function_limit& function);
    ~Function_limit() override;
    Function_limit& operator=(const Function_limit&) = default;

    // Type 1: First and second param define the range (min,max) and the rest belongs to data.
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;


    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;
};


}  // namespace Functions
}  // namespace SmartMet
