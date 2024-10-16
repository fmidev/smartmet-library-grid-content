#include "Function_mul.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_mul::Function_mul()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_mul::Function_mul(const Function_mul& function)
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





Function_mul::~Function_mul()
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





float Function_mul::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    float value = 1;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        value = value * val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_mul::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    double value = 1;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        value = value * val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_mul::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    uint elen = extParameters.size();
    outParameters.reserve(sz);

    float b = 1;
    if (elen > 0)
    {
      for (uint t=0; t<elen; t++)
        b = b * extParameters[t];
    }

    for (uint s=0; s<sz; s++)
    {
      float value = 1;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          float val = inParameters[t][s];
          if (val != ParamValueMissing)
            value = value * val;
        }
      }
      if (b != 1)
        value = value * b;

      outParameters.emplace_back(value);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_mul::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    uint elen = extParameters.size();
    outParameters.reserve(sz);

    double b = 1;
    if (elen > 0)
    {
      for (uint t=0; t<elen; t++)
        b = b * extParameters[t];
    }

    for (uint s=0; s<sz; s++)
    {
      double value = 1;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (val != ParamValueMissing)
            value = value * val;
        }
      }
      if (b != 1)
        value = value * b;

      outParameters.emplace_back(value);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_mul::duplicate()
{
  try
  {
    return (Function*)new Function_mul(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
