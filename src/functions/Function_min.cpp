#include "Function_min.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_min::Function_min()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_min::Function_min(const Function_min& function)
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





Function_min::~Function_min()
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





float Function_min::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    float min = ParamValueMissing;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (min == ParamValueMissing || val < min)
        min = val;
    }
    return min;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_min::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double min = ParamValueMissing;
    uint len = parameters.size();
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (min == ParamValueMissing || val < min)
        min = val;
    }
    return min;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_min::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float min = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          float val = inParameters[t][s];
          if (min == ParamValueMissing || val < min)
            min = val;
        }
      }
      outParameters.push_back(min);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_min::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double min = ParamValueMissing;
      for (uint t=0; t<len; t++)
      {
        if (s < inParameters[t].size())
        {
          double val = inParameters[t][s];
          if (min == ParamValueMissing || val < min)
            min = val;
        }
      }
      outParameters.push_back(min);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_min::duplicate()
{
  try
  {
    return (Function*)new Function_min(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
