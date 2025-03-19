#include "Function_and.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_and::Function_and()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_and::Function_and(const Function_and& function)
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





Function_and::~Function_and()
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





float Function_and::executeFunctionCall1(std::vector<float>& parameters)
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

      if (parameters[t] == 0)
        return 0;
    }
    return 1;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_and::executeFunctionCall1(std::vector<double>& parameters)
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

      if (parameters[t] == 0)
        return 0;
    }
    return 1;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_and::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
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
        float val = 1;
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

          if (val1 == 0)
          {
            val = 0;
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





void Function_and::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
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
        float val = 1;
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

          if (val1 == 0)
          {
            val = 0;
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




Function* Function_and::duplicate()
{
  try
  {
    return (Function*)new Function_and(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
