#pragma once

#include <grid-files/common/Coordinate.h>
#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace Functions
{

const double degreesToRadiansMultiplier = 0.0174532925199;
const double radiansToDegreesMultiplier = 57.2957795131;


class Function
{
  public:
                      Function();
                      Function(const Function& function);
    virtual           ~Function();

    // Type 1: Takes one or several parameters, returns one value
    virtual float     executeFunctionCall1(std::vector<float>& parameters);
    virtual double    executeFunctionCall1(std::vector<double>& parameters);

    // Type 4: Takes two data vectors and angle vector, returns one vector
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    virtual void      executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    virtual void      executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    virtual Function* duplicate();
    virtual void      print(std::ostream& stream,uint level,uint optionFlags);
};


typedef Function* FunctionPtr;
typedef std::vector<FunctionPtr> FunctionPtr_vec;


}  // namespace Functions
}  // namespace SmartMet
