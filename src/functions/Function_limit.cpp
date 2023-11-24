#include "Function_limit.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_limit::Function_limit()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_limit::Function_limit(const Function_limit& function)
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





Function_limit::~Function_limit()
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





float Function_limit::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    // Parameters : min,max,value

    if (parameters.size() != 3)
      return ParamValueMissing;

    if (parameters[2] < parameters[0])
      return ParamValueMissing;

    if (parameters[2] > parameters[1])
      return ParamValueMissing;

    return parameters[2];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




double Function_limit::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    // Parameters : min,max,value

    if (parameters.size() != 3)
      return ParamValueMissing;

    if (parameters[2] < parameters[0])
      return ParamValueMissing;

    if (parameters[2] > parameters[1])
      return ParamValueMissing;

    return parameters[2];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_limit::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float val = ParamValueMissing;
      if (s < inParameters[0].size())
        val = inParameters[0][s];

      if (val != ParamValueMissing)
      {
        if (val < min || val > max)
          val = ParamValueMissing;
      }
      outParameters.emplace_back(val);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_limit::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float val = ParamValueMissing;
      if (s < inParameters[0].size())
        val = inParameters[0][s];

      if (val != ParamValueMissing)
      {
        if (val < min || val > max)
          val = ParamValueMissing;
      }
      outParameters.emplace_back(val);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_limit::duplicate()
{
  try
  {
    return (Function*)new Function_limit(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
