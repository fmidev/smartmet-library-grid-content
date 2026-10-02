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
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        return val;
    }
    return ParamValueMissing;
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
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        return val;
    }
    return ParamValueMissing;
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
    // The first valid value at each grid point. (The first non-empty grid used to be returned
    // as such, with its missing values.)
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.clear();
    outParameters.reserve(sz);
    for (uint s=0; s<sz; s++)
    {
      double val = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size() && inParameters[t][s] != ParamValueMissing)
        {
          val = inParameters[t][s];
          break;
        }
      }
      outParameters.emplace_back(val);
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
    // The first valid value at each grid point. (The first non-empty grid used to be returned
    // as such, with its missing values.)
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.clear();
    outParameters.reserve(sz);
    for (uint s=0; s<sz; s++)
    {
      double val = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size() && inParameters[t][s] != ParamValueMissing)
        {
          val = inParameters[t][s];
          break;
        }
      }
      outParameters.emplace_back(val);
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
