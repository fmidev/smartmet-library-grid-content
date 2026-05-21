#include "Function.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


/*! \brief Function: Constructor. */

Function::Function()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Constructor. */

Function::Function(const Function& function)
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Destructor. */

Function::~Function()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





/*! \brief Function: Execute function call1. */

float Function::executeFunctionCall1(std::vector<float>& parameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}





/*! \brief Function: Execute function call1. */

double Function::executeFunctionCall1(std::vector<double>& parameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}





/*! \brief Function: Execute function call4. */

void Function::executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}





/*! \brief Function: Execute function call4. */

void Function::executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}




/*! \brief Function: Execute function call9. */

void Function::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}





/*! \brief Function: Execute function call9. */

void Function::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  throw Fmi::Exception(BCP, "Not implemented!");
}





/*! \brief Function: Duplicate. */

Function* Function::duplicate()
{
  try
  {
    return (Function*)new Function(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Print. */

void Function::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
