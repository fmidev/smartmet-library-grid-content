#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Computes the percentage of ensemble members whose value falls outside the specified [min, max] range. */
// ====================================================================================

class Function_outPrcnt : public Function
{
  public:
                      Function_outPrcnt();
                      Function_outPrcnt(const Function_outPrcnt& function);
    ~Function_outPrcnt() override;
    Function_outPrcnt& operator=(const Function_outPrcnt&) = default;

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
