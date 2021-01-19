#include "Function_add.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_add::Function_add()
{
  try
  {
    mValueToAdd = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_add::Function_add(double valueToAdd)
{
  try
  {
    mValueToAdd = valueToAdd;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_add::Function_add(const Function_add& function)
:Function(function)
{
  try
  {
    mValueToAdd = function.mValueToAdd;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function_add::~Function_add()
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





float Function_add::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
        return parameters[0] + mValueToAdd;
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





double Function_add::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
        return parameters[0] + mValueToAdd;
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





void Function_add::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
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
        float c = (float)(a + mValueToAdd);
        outParameters.emplace_back(c);
      }
      else
      {
        outParameters.emplace_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_add::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
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
        double c = (double)(a + mValueToAdd);
        outParameters.emplace_back(c);
      }
      else
      {
        outParameters.emplace_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_add::duplicate()
{
  try
  {
    return (Function*)new Function_add(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
