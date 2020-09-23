#include "Function_div.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


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





float Function_div::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
        return parameters[0] / parameters[1];
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_div::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing && parameters[1] != ParamValueMissing)
        return parameters[0] / parameters[1];
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*
void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = inParameters[0][t];

      if (a != ParamValueMissing)
      {
        float c = (float)(a * mmuler);
        outParameters.push_back(c);
      }
      else
      {
        outParameters.push_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() != 1)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = inParameters[0][t];

      if (a != ParamValueMissing)
      {
        double c = (double)(a * mmuler);
        outParameters.push_back(c);
      }
      else
      {
        outParameters.push_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}
*/


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
