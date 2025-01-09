#include "Function_smedian.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_smedian::Function_smedian()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_smedian::Function_smedian(const Function_smedian& function)
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





Function_smedian::~Function_smedian()
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





float Function_smedian::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();

    if (len < 2)
      return ParamValueMissing;

    int shift = (int)parameters[0];

    std::vector<float> values;
    values.reserve(len);

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        values.push_back(val);
    }
    std::sort(values.begin(),values.end());

    int sz = values.size();
    if (sz == 0)
      return ParamValueMissing;

    int idx = (sz + 1) / 2;

    int i = idx - 1 + shift;

    if ((sz % 2) == 1)
    {
      if (i < 0)
        i = 0;

      if (i >= sz)
        i = sz-1;

      return values[i];
    }

    if (i >= (sz-1))
      i = sz-2;

    if (i < 0)
      i = 0;

    return (values[i]+values[i+1])/2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_smedian::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();

    if (len < 2)
      return ParamValueMissing;

    int shift = (int)parameters[0];

    std::vector<double> values;
    values.reserve(len);

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
        values.push_back(val);
    }
    std::sort(values.begin(),values.end());

    int sz = values.size();
    if (sz == 0)
      return ParamValueMissing;

    int idx = (sz + 1) / 2;

    int i = idx - 1 + shift;

    if ((sz % 2) == 1)
    {
      if (i < 0)
        i = 0;

      if (i >= sz)
        i = sz-1;

      return values[i];
    }

    if (i >= (sz-1))
      i = sz-2;

    if (i < 0)
      i = 0;

    return (values[i]+values[i+1])/2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_smedian::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    if (extParameters.size() != 1)
      return;

    int shift = (int)extParameters[0];
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
      int vsz = values.size();
      if (vsz > 0)
      {
        int idx = (vsz + 1) / 2;

        int i = idx - 1 + shift;

        if ((vsz % 2) == 1)
        {
          if (i < 0)
            i = 0;

          if (i >= sz)
            i = sz-1;

          res = values[i];
        }
        else
        {
          if (i >= (sz-1))
            i = sz-2;

          if (i < 0)
            i = 0;

          res = (values[i]+values[i+1])/2;
        }
      }
      outParameters.emplace_back(res);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_smedian::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    if (extParameters.size() != 1)
      return;

    int shift = (int)extParameters[0];

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
      int vsz = values.size();
      if (vsz > 0)
      {
        int idx = (vsz + 1) / 2;
        int i = idx - 1 + shift;

        if ((vsz % 2) == 1)
        {
          if (i < 0)
            i = 0;

          if (i >= sz)
            i = sz-1;

          res = values[i];
        }
        else
        {
          if (i >= (sz-1))
            i = sz-2;

          if (i < 0)
            i = 0;

          res = (values[i]+values[i+1])/2;
        }
      }
      outParameters.emplace_back(res);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_smedian::duplicate()
{
  try
  {
    return (Function*)new Function_smedian(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
