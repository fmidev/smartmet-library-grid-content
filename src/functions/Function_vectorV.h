#pragma once

#include "Function.h"


namespace SmartMet
{
namespace Functions
{


class Function_vectorV : public Function
{
  public:
                      Function_vectorV();
                      Function_vectorV(const Function_vectorV& function);
    virtual           ~Function_vectorV();

    // Type 4: Takes two data vectors and angle vector, returns one vector
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    virtual Function* duplicate();
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);
};


}  // namespace Functions
}  // namespace SmartMet
