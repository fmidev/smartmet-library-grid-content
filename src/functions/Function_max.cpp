#include "Function_max.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_max::Function_max()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_max::Function_max(const Function_max& function)
:Function(function)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_max::~Function_max()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





float Function_max::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    float max = ParamValueMissing;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (max == ParamValueMissing || val > max)
        max = val;
    }
    return max;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_max::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double max = ParamValueMissing;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (max == ParamValueMissing || val > max)
        max = val;
    }
    return max;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_max::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float max = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          float val = inParameters[t][s];
          if (max == ParamValueMissing || val > max)
            max = val;
        }
      }
      outParameters.push_back(max);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_max::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double max = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (max == ParamValueMissing || val > max)
            max = val;
        }
      }
      outParameters.push_back(max);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_max::duplicate()
{
  try
  {
    return (Function*)new Function_max(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
