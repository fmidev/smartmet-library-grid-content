#include "Function_avg.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_avg::Function_avg()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_avg::Function_avg(const Function_avg& function)
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





Function_avg::~Function_avg()
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





float Function_avg::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    double total = 0;
    uint len = parameters.size();
    uint cnt = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        total = total + val;
        cnt++;
      }
    }
    if (cnt > 0)
      return total/cnt;

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_avg::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double total = 0;
    uint len = parameters.size();
    uint cnt = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        total = total + val;
        cnt++;
      }
    }
    if (cnt > 0)
      return total/cnt;

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_avg::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double total = 0;
      uint cnt = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          total = total + val;
          cnt++;
        }
      }
      if (cnt > 0)
        outParameters.push_back(total/cnt);
      else
        outParameters.push_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_avg::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double total = 0;
      uint cnt = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          total = total + val;
          cnt++;
        }
      }
      if (cnt > 0)
        outParameters.push_back(total/cnt);
      else
        outParameters.push_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_avg::duplicate()
{
  try
  {
    return (Function*)new Function_avg(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
