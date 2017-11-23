#include "Function.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function::Function()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function::Function(const Function& function)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




Function::~Function()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function::executeFunctionType1(std::vector<float>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





double Function::executeFunctionType1(std::vector<double>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType2(std::vector<double>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType2(std::vector<float>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType3(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionType3(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}




Function* Function::duplicate()
{
  try
  {
    return (Function*)new Function(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void Function::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}


}  // namespace Functions
}  // namespace SmartMet
