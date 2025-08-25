#include "Function_replace.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_replace::Function_replace()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_replace::Function_replace(const Function_replace& function)
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





Function_replace::~Function_replace()
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





float Function_replace::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    // Parameters : paramValue,range_border,replaceValue,range_border,[... ,replaceValue,range_border]

    if (parameters.size() < 1)
      return ParamValueMissing;

    double val = parameters[0];
    uint sz = parameters.size();
    for (uint t=1; t<sz; t = t + 2)
    {
      if ((t + 2) < sz)
      {
        if (val >= parameters[t]  &&  val < parameters[t+2])
          return parameters[t+1];
      }
    }
    return val;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




double Function_replace::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    // Parameters : paramValue,range_border,replaceValue,range_border,[... ,replaceValue,range_border]

    if (parameters.size() < 1)
      return ParamValueMissing;

    double val = parameters[0];
    uint sz = parameters.size();
    for (uint t=1; t<sz; t = t + 2)
    {
      if ((t + 2) < sz)
      {
        if (val >= parameters[t]  &&  val < parameters[t+2])
          return parameters[t+1];
      }
    }
    return val;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_replace::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (extParameters.size() < 1)
      return;

    uint esz = extParameters.size();
    uint sz = columns*rows;
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float val = ParamValueMissing;
      if (s < inParameters[0].size())
        val = inParameters[0][s];

      float newVal = val;
      for (uint t=0; t<esz; t = t + 2)
      {
        if ((t + 2) < esz)
        {
          if (val >= extParameters[t]  &&  val <= extParameters[t+2])
          {
            newVal = extParameters[t+1];
            t = esz;
          }
        }
      }
      outParameters.emplace_back(newVal);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_replace::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (extParameters.size() < 1)
      return;

    uint esz = extParameters.size();
    uint sz = columns*rows;
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float val = ParamValueMissing;
      if (s < inParameters[0].size())
        val = inParameters[0][s];

      float newVal = val;

      for (uint t=0; t<esz; t = t + 2)
      {
        if ((t + 2) < esz)
        {
          if (val >= extParameters[t]  &&  val <= extParameters[t+2])
          {
            newVal = extParameters[t+1];
            t = esz;
          }
        }
      }
      outParameters.emplace_back(newVal);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_replace::duplicate()
{
  try
  {
    return (Function*)new Function_replace(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
