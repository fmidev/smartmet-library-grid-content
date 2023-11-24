#include "Function_median.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_median::Function_median()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_median::Function_median(const Function_median& function)
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





Function_median::~Function_median()
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





float Function_median::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();

    std::vector<float> values;
    values.reserve(len);

    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        values.push_back(val);
    }
    std::sort(values.begin(),values.end());

    uint sz = values.size();
    if (sz == 0)
      return ParamValueMissing;

    uint idx = (sz + 1) / 2;
    if ((sz % 2) == 1)
      return values[idx-1];

    return (values[idx-1]+values[idx])/2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_median::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();

    std::vector<double> values;
    values.reserve(len);

    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        values.push_back(val);
    }
    std::sort(values.begin(),values.end());

    uint sz = values.size();
    if (sz == 0)
      return ParamValueMissing;

    uint idx = (sz + 1) / 2;
    if ((sz % 2) == 1)
      return values[idx-1];

    return (values[idx-1]+values[idx])/2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_median::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::vector<float> values;
      values.reserve(len);

      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (val != ParamValueMissing)
            values.push_back(val);
        }
      }

      std::sort(values.begin(),values.end());

      double res = ParamValueMissing;
      uint vsz = values.size();
      if (vsz > 0)
      {
        uint idx = (vsz + 1) / 2;
        if ((vsz % 2) == 1)
          res = values[idx-1];
        else
          res = (values[idx-1]+values[idx])/2;
      }
      outParameters.emplace_back(res);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_median::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::vector<double> values;
      values.reserve(len);

      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (val != ParamValueMissing)
            values.push_back(val);
        }
      }

      std::sort(values.begin(),values.end());

      double res = ParamValueMissing;
      uint vsz = values.size();
      if (vsz > 0)
      {
        uint idx = (vsz + 1) / 2;
        if ((vsz % 2) == 1)
          res = values[idx-1];
        else
          res = (values[idx-1]+values[idx])/2;
      }
      outParameters.emplace_back(res);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_median::duplicate()
{
  try
  {
    return (Function*)new Function_median(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
