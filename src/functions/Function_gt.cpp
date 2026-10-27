#include "Function_gt.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



/*! \brief Function: Constructor. */

Function_gt::Function_gt()
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

Function_gt::Function_gt(const Function_gt& function)
:Function(function)
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

Function_gt::~Function_gt()
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

float Function_gt::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        if (parameters[0] > parameters[1])
          return 1.0;

        return 0.0;
      }
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_gt::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        if (parameters[0] > parameters[1])
          return 1.0;

        return 0.0;
      }
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_gt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    compareGridValues(columns,rows,inParameters,extParameters,[](float val1,float val2) { return val1 > val2; },outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_gt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    compareGridValues(columns,rows,inParameters,extParameters,[](float val1,float val2) { return val1 > val2; },outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_gt::duplicate()
{
  try
  {
    return (Function*)new Function_gt(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
