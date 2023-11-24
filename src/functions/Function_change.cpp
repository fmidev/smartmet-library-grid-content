#include "Function_change.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_change::Function_change()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_change::Function_change(const Function_change& function)
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





Function_change::~Function_change()
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





float Function_change::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len < 2)
      return ParamValueMissing;

    return parameters[len-1] - parameters[0];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_change::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len < 2)
      return ParamValueMissing;

    return parameters[len-1] - parameters[0];
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_change::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double val = ParamValueMissing;
      if (len >= 2)
      {
        if (s < inParameters[len-1].size() &&  s < inParameters[0].size())
          val = inParameters[len-1][s] - inParameters[0][s];
      }
      outParameters.emplace_back(val);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_change::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double val = ParamValueMissing;
      if (len >= 2)
      {
        if (s < inParameters[len-1].size() &&  s < inParameters[0].size())
          val = inParameters[len-1][s] - inParameters[0][s];
      }
      outParameters.emplace_back(val);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_change::duplicate()
{
  try
  {
    return (Function*)new Function_change(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
