#include "Function_sum.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_sum::Function_sum()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_sum::Function_sum(const Function_sum& function)
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





Function_sum::~Function_sum()
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





float Function_sum::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    float sum = 0;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        sum += val;
    }
    return sum;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_sum::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double sum = 0;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        sum += val;
    }
    return sum;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_sum::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    double b = 0;
    if (extParameters.size() > 0)
    {
      for (auto it=extParameters.begin(); it != extParameters.end();++it)
        b = b + *it;
    }

    for (uint s=0; s<sz; s++)
    {
      double sum = 0;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (val != ParamValueMissing)
            sum += val;
        }
      }
      sum += b;
      outParameters.emplace_back(sum);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_sum::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    double b = 0;
    if (extParameters.size() > 0)
    {
      for (auto it=extParameters.begin(); it != extParameters.end();++it)
        b = b + *it;
    }

    for (uint s=0; s<sz; s++)
    {
      double sum = 0;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (val != ParamValueMissing)
            sum += val;
        }
      }
      sum += b;
      outParameters.emplace_back(sum);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_sum::duplicate()
{
  try
  {
    return (Function*)new Function_sum(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
