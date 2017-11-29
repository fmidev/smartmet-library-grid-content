#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace Functions
{


class Function
{
  public:
                      Function();
                      Function(const Function& function);
    virtual           ~Function();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionType1(std::vector<float>& parameters);
    virtual double    executeFunctionType1(std::vector<double>& parameters);

    // Type 2: Takes and returns an vector
    virtual void      executeFunctionType2(std::vector<float>& inOutParameters);
    virtual void      executeFunctionType2(std::vector<double>& inOutParameters);
    virtual void      executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters);

    // Type 3: Takes two vectors and returns one vector
    virtual void      executeFunctionType3(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters);
    virtual void      executeFunctionType3(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters);

    virtual Function* duplicate();
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);
};


typedef Function* FunctionPtr;
typedef std::vector<FunctionPtr> FunctionPtr_vec;


}  // namespace Functions
}  // namespace SmartMet