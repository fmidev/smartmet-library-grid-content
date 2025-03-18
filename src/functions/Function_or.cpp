#include "Function_or.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_or::Function_or()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_or::Function_or(const Function_or& function)
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





Function_or::~Function_or()
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





float Function_or::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint sz = parameters.size();
    if (sz < 2)
      throw Fmi::Exception(BCP, "The parameters vector should contain at least two values!");

    for (uint t=0; t<sz; t++)
    {
      if (parameters[t] == ParamValueMissing)
        return ParamValueMissing;

      if (parameters[t] != 0)
        return 1.0;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_or::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint sz = parameters.size();
    if (sz < 2)
      throw Fmi::Exception(BCP, "The parameters vector should contain at least two values!");

    for (uint t=0; t<sz; t++)
    {
      if (parameters[t] == ParamValueMissing)
        return ParamValueMissing;

      if (parameters[t] != 0)
        return 1.0;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_or::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);
    uint len = inParameters.size();

    if (len >= 2)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = 0;
        for (uint t=0; t<len; t++)
        {
          float val1 = ParamValueMissing;
          if (s < inParameters[t].size())
            val1 = inParameters[t][s];

          if (val1 == ParamValueMissing)
          {
            val = ParamValueMissing;
            t = len;
          }

          if (val1 != 0)
          {
            val = 1.0;
            t = len;
          }
        }
        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_or::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);
    uint len = inParameters.size();

    if (len >= 2)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = 0;
        for (uint t=0; t<len; t++)
        {
          float val1 = ParamValueMissing;
          if (s < inParameters[t].size())
            val1 = inParameters[t][s];

          if (val1 == ParamValueMissing)
          {
            val = ParamValueMissing;
            t = len;
          }

          if (val1 != 0)
          {
            val = 1.0;
            t = len;
          }
        }
        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_or::duplicate()
{
  try
  {
    return (Function*)new Function_or(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
