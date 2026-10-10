#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


// ====================================================================================
/*! \brief Computes the apparent temperature (feels-like index) from wind, humidity, temperature, and radiation. */
// ====================================================================================

class Function_feelsLike : public Function
{
  public:
                      Function_feelsLike();
                      Function_feelsLike(const Function_feelsLike& function);
    ~Function_feelsLike() override;

    // Type 1: Takes one or several parameters, returns one value
    float     executeFunctionCall1(std::vector<float>& parameters) override;
    double    executeFunctionCall1(std::vector<double>& parameters) override;

    // Type 9: Takes 1 to N vectors and returns one vector
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters) override;
    void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters) override;

    Function* duplicate() override;
    void      print(std::ostream& stream,uint level,uint optionFlags) override;

  protected:

    double            feelsLike(double wind,double rh,double temp,double rad);
    double            SSI(double rh,double t);

};



}  // namespace Functions
}  // namespace SmartMet
