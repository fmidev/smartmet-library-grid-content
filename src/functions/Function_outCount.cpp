#include "Function_outCount.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_outCount::Function_outCount()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_outCount::Function_outCount(const Function_outCount& function)
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





Function_outCount::~Function_outCount()
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





float Function_outCount::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint count = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val < min  ||  val > max)
        count++;
    }

    return (float)count;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




double Function_outCount::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint count = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val < min  ||  val > max)
        count++;
    }

    return (double)count;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_outCount::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      uint count = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val < min  ||  val > max)
          count++;
      }
      outParameters.emplace_back(count);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_outCount::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      uint count = 0;
      for (uint t=0; t<len; t++)
      {
        double val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val < min  ||  val > max)
          count++;
      }
      outParameters.emplace_back((double)count);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_outCount::duplicate()
{
  try
  {
    return (Function*)new Function_outCount(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
