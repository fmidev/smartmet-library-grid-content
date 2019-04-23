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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function::Function(const Function& function)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function::~Function()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





float Function::executeFunctionCall1(std::vector<float>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





double Function::executeFunctionCall1(std::vector<double>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall3(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall3(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function::executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
