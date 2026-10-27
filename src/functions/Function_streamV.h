#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Rotates the northward (V) stream-line component to the target grid orientation. */
// ====================================================================================

class Function_streamV : public Function
{
  public:
                      Function_streamV();
                      Function_streamV(const Function_streamV& function);
    ~Function_streamV() override;
    Function_streamV& operator=(const Function_streamV&) = default;

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;

    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;
    void      print(std::ostream& stream,uint level,uint optionFlags) override;
};


}  // namespace Functions
}  // namespace SmartMet
