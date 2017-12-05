#include "Function_add.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_add::Function_add()
{
  try
  {
    mValueToAdd = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_add::Function_add(double valueToAdd)
{
  try
  {
    mValueToAdd = valueToAdd;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_add::Function_add(const Function_add& function)
:Function(function)
{
  try
  {
    mValueToAdd = function.mValueToAdd;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




Function_add::~Function_add()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function_add::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] + mValueToAdd;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





double Function_add::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] + mValueToAdd;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inOutParameters)
{
  try
  {
    uint len = inOutParameters.size();
    for (uint t=0; t<len; t++)
    {
      inOutParameters[t] += mValueToAdd;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    outParameters.reserve(inParameters.size());
    for (auto it = inParameters.begin();  it != inParameters.end(); ++it)
    {
      outParameters.push_back(*it + mValueToAdd);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    outParameters.reserve(inParameters.size());
    for (auto it = inParameters.begin();  it != inParameters.end(); ++it)
    {
      outParameters.push_back(*it + mValueToAdd);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inOutParameters)
{
  try
  {
    uint len = inOutParameters.size();
    for (uint t=0; t<len; t++)
    {
      inOutParameters[t] += mValueToAdd;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall3(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    uint len = inParameters1.size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = inParameters1[t];
      double b = inParameters2[t];

      float c = (float)a+b;
      outParameters.push_back(c);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_add::executeFunctionCall3(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  uint len = inParameters1.size();
  outParameters.reserve(len);
  for (uint t=0; t<len; t++)
  {
    double a = inParameters1[t];
    double b = inParameters2[t];

    double c = a+b;
    outParameters.push_back(c);
  }
}





Function* Function_add::duplicate()
{
  try
  {
    return (Function*)new Function_add(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace Functions
}  // namespace SmartMet
