#include "Function_diff.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


namespace
{

/*! \brief Return the sum of the external parameters, or missing if any of them is missing. */

template <typename T>
T sumOfParameters(const std::vector<double>& extParameters)
{
  T b = 0;
  for (double p : extParameters)
  {
    if (p == ParamValueMissing)
      return ParamValueMissing;

    b = b + p;
  }
  return b;
}



/*! \brief Subtract the values of the other grids at the index from the value of the first grid. Returns missing if a value is missing. */

template <typename T>
T subtractGridValues(const std::vector<std::vector<T>>& inParameters,uint s)
{
  // (A missing first value used to be subtracted from as a number)
  T value = inParameters[0][s];
  if (value == ParamValueMissing)
    return value;

  uint len = inParameters.size();
  for (uint t=1; t<len; t++)
  {
    if (s >= inParameters[t].size())
      return ParamValueMissing;

    float val = inParameters[t][s];
    if (val == ParamValueMissing)
      return ParamValueMissing;

    value -= val;
  }
  return value;
}



/*! \brief Subtract the other grids and the sum of the external parameters from the first grid. */

template <typename T>
void executeDiff(uint columns,uint rows,const std::vector<std::vector<T>>& inParameters,const std::vector<double>& extParameters,std::vector<T>& outParameters)
{
  uint sz = columns*rows;
  outParameters.reserve(sz);

  T b = sumOfParameters<T>(extParameters);

  for (uint s=0; s<sz; s++)
  {
    T value = ParamValueMissing;
    if (s < inParameters[0].size())
      value = subtractGridValues(inParameters,s);

    if (b == ParamValueMissing)
      value = ParamValueMissing;
    else if (value != ParamValueMissing)
      value -= b;

    outParameters.emplace_back(value);
  }
}

}  // namespace



/*! \brief Function: Constructor. */

Function_diff::Function_diff()
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

Function_diff::Function_diff(const Function_diff& function)
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

Function_diff::~Function_diff()
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

float Function_diff::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    float value = parameters[0];
    if (value == ParamValueMissing)
      return ParamValueMissing;

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val == ParamValueMissing)
        return ParamValueMissing;

      value -= val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_diff::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    double value = parameters[0];
    if (value == ParamValueMissing)
      return ParamValueMissing;

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val == ParamValueMissing)
        return ParamValueMissing;

      value -= val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_diff::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    executeDiff(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_diff::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    executeDiff(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_diff::duplicate()
{
  try
  {
    return (Function*)new Function_diff(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
