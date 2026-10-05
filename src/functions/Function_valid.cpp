#include "Function_valid.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


/*! \brief Function: Constructor. */

Function_valid::Function_valid()
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

Function_valid::Function_valid(const Function_valid& function)
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

Function_valid::~Function_valid()
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

float Function_valid::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    // The first parameter as such, including a missing value, like the grid version returns
    // the first available grid. The query server leaves out the parameters without data.
    if (parameters.empty())
      return ParamValueMissing;
    return parameters[0];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_valid::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    // The first parameter as such, including a missing value, like the grid version returns
    // the first available grid. The query server leaves out the parameters without data.
    if (parameters.empty())
      return ParamValueMissing;
    return parameters[0];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_valid::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    // The first available grid is returned as such, including its missing values. The
    // parameters are listed in order of preference, typically a nowcast first and a longer
    // range forecast after it, and a missing value in the preferred grid is valid information
    // (for example no precipitation) which must not be filled from the less reliable grids.
    uint len = inParameters.size();
    for (uint t=0; t<len; t++)
    {
      if (inParameters[t].size())
      {
        outParameters = inParameters[t];
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_valid::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    // The first available grid is returned as such, including its missing values. The
    // parameters are listed in order of preference, typically a nowcast first and a longer
    // range forecast after it, and a missing value in the preferred grid is valid information
    // (for example no precipitation) which must not be filled from the less reliable grids.
    uint len = inParameters.size();
    for (uint t=0; t<len; t++)
    {
      if (inParameters[t].size())
      {
        outParameters = inParameters[t];
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_valid::duplicate()
{
  try
  {
    return (Function*)new Function_valid(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
