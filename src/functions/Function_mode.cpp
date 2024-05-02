#include "Function_mode.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_mode::Function_mode()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_mode::Function_mode(const Function_mode& function)
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





Function_mode::~Function_mode()
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





float Function_mode::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() < 2)
      return ParamValueMissing;

    int decimals = parameters[0];
    double mul = 1.0;
    if (decimals > 0)
      mul = pow(10.0,decimals);

    std::map<long long,int> valueCount;

    uint count = 0;

    uint len = parameters.size();
    for (uint s=1; s<len; s++)
    {
      double val = parameters[s];
      if (val != ParamValueMissing)
      {
        long long key = (long long)(val*mul + 0.5);
        auto it = valueCount.find(key);
        if (it != valueCount.end())
          it->second++;
        else
          valueCount.insert(std::pair<long long,int>(key,1));
      }
    }

    if (valueCount.size() == 0)
      return ParamValueMissing;

    int maxVal = 0;
    long long maxKey = 0;

    for (auto it = valueCount.begin();it != valueCount.end(); ++it)
    {
      if (it->second > maxVal)
      {
        maxVal = it->second;
        maxKey = it->first;
      }
    }

    return (float)maxKey/mul;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




double Function_mode::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() < 2)
      return ParamValueMissing;

    int decimals = parameters[0];
    double mul = 1.0;
    if (decimals > 0)
      mul = pow(10.0,decimals);

    std::map<long long,int> valueCount;

    uint count = 0;

    uint len = parameters.size();
    for (uint s=1; s<len; s++)
    {
      double val = parameters[s];
      if (val != ParamValueMissing)
      {
        long long key = (long long)(val*mul + 0.5);
        auto it = valueCount.find(key);
        if (it != valueCount.end())
          it->second++;
        else
          valueCount.insert(std::pair<long long,int>(key,1));
      }
    }

    if (valueCount.size() == 0)
      return ParamValueMissing;

    int maxVal = 0;
    long long maxKey = 0;

    for (auto it = valueCount.begin();it != valueCount.end(); ++it)
    {
      if (it->second > maxVal)
      {
        maxVal = it->second;
        maxKey = it->first;
      }
    }

    return (double)maxKey/mul;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_mode::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (extParameters.size() != 1)
      return;

    int decimals = extParameters[0];
    double mul = 1.0;
    if (decimals > 0)
      mul = pow(10.0,decimals);

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::map<long long,int> valueCount;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
        {
          val = inParameters[t][s];

          long long key = (long long)(val*mul + 0.5);
          auto it = valueCount.find(key);
          if (it != valueCount.end())
            it->second++;
          else
            valueCount.insert(std::pair<long long,int>(key,1));
        }
      }

      if (valueCount.size() == 0)
      {
        outParameters.emplace_back(ParamValueMissing);
      }
      else
      {
        int maxVal = 0;
        long long maxKey = 0;

        for (auto it = valueCount.begin();it != valueCount.end(); ++it)
        {
          if (it->second > maxVal)
          {
            maxVal = it->second;
            maxKey = it->first;
          }
        }

        outParameters.emplace_back((double)maxKey/mul);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_mode::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (extParameters.size() != 1)
      return;

    int decimals = extParameters[0];
    double mul = 1.0;
    if (decimals > 0)
      mul = pow(10.0,decimals);

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::map<long long,int> valueCount;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
        {
          val = inParameters[t][s];

          long long key = (long long)(val*mul + 0.5);
          auto it = valueCount.find(key);
          if (it != valueCount.end())
            it->second++;
          else
            valueCount.insert(std::pair<long long,int>(key,1));
        }
      }

      if (valueCount.size() == 0)
      {
        outParameters.emplace_back(ParamValueMissing);
      }
      else
      {
        int maxVal = 0;
        long long maxKey = 0;

        for (auto it = valueCount.begin();it != valueCount.end(); ++it)
        {
          if (it->second > maxVal)
          {
            maxVal = it->second;
            maxKey = it->first;
          }
        }

        outParameters.emplace_back((double)maxKey/mul);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_mode::duplicate()
{
  try
  {
    return (Function*)new Function_mode(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
