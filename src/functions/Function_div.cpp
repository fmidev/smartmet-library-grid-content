#include "Function_div.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


namespace
{

/*! \brief Return the product of the external parameters, or missing if any of them is missing. */

template <typename T>
T productOfParameters(const std::vector<double>& extParameters)
{
  T b = 1;
  for (double p : extParameters)
  {
    if (p == ParamValueMissing)
      return ParamValueMissing;

    b = b * p;
  }
  return b;
}



/*! \brief Divide the value of the first grid at the index by the values of the other grids. Returns missing if a value is missing. */

template <typename T>
T divideGridValues(const std::vector<std::vector<T>>& inParameters,uint s)
{
  T value = inParameters[0][s];
  if (value == ParamValueMissing)
    return value;

  uint len = inParameters.size();
  for (uint t=1; t<len; t++)
  {
    if (s >= inParameters[t].size())
      return ParamValueMissing;

    double val = inParameters[t][s];
    if (val == ParamValueMissing)
      return ParamValueMissing;

    // If the divisor is zero then we ignore the division. Should we return ParamValueMissing instead?
    if (val != 0)
      value = value / val;
  }
  return value;
}



/*! \brief Divide the first grid by the other grids and by the product of the external parameters. */

template <typename T>
void executeDiv(uint columns,uint rows,const std::vector<std::vector<T>>& inParameters,const std::vector<double>& extParameters,std::vector<T>& outParameters)
{
  uint sz = columns*rows;
  outParameters.reserve(sz);

  T b = productOfParameters<T>(extParameters);

  for (uint s=0; s<sz; s++)
  {
    T value = ParamValueMissing;
    if (s < inParameters[0].size())
    {
      value = divideGridValues(inParameters,s);

      if (b == ParamValueMissing)
        value = ParamValueMissing;

      if (b != 1  &&  b != 0  &&  value != ParamValueMissing)
        value = value / b;
    }

    outParameters.emplace_back(value);
  }
}

}  // namespace



/*! \brief Function: Constructor. */

Function_div::Function_div()
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

Function_div::Function_div(const Function_div& function)
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

Function_div::~Function_div()
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

float Function_div::executeFunctionCall1(std::vector<float>& parameters)
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

      if (val != 0)
        value = value / val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_div::executeFunctionCall1(std::vector<double>& parameters)
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

      if (val != 0)
        value = value / val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    executeDiv(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    executeDiv(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_div::duplicate()
{
  try
  {
    return (Function*)new Function_div(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
