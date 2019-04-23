#include "Function_multiply.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_multiply::Function_multiply(double multiplyer)
{
  try
  {
    mMultiplyer = multiplyer;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_multiply::Function_multiply(const Function_multiply& function)
:Function(function)
{
  try
  {
    mMultiplyer = function.mMultiplyer;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function_multiply::~Function_multiply()
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





float Function_multiply::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] * mMultiplyer;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_multiply::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] * mMultiplyer;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inOutParameters)
{
  try
  {
    uint len = inOutParameters.size();
    for (uint t=0; t<len; t++)
    {
      inOutParameters[t] *= mMultiplyer;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall2(uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    outParameters.clear();
    outParameters.reserve(inParameters.size());
    for (auto it = inParameters.begin();  it != inParameters.end(); ++it)
    {
      outParameters.push_back(*it * mMultiplyer);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    outParameters.clear();
    outParameters.reserve(inParameters.size());
    for (auto it = inParameters.begin();  it != inParameters.end(); ++it)
    {
      outParameters.push_back(*it * mMultiplyer);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall2(uint columns,uint rows,std::vector<float>& inOutParameters)
{
  try
  {
    uint len = inOutParameters.size();
    for (uint t=0; t<len; t++)
    {
      inOutParameters[t] *= mMultiplyer;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall3(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    uint len = inParameters1.size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = inParameters1[t];
      double b = inParameters2[t];

      float c = (float)a*b;
      outParameters.push_back(c);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_multiply::executeFunctionCall3(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  uint len = inParameters1.size();
  outParameters.reserve(len);
  for (uint t=0; t<len; t++)
  {
    double a = inParameters1[t];
    double b = inParameters2[t];

    double c = a*b;
    outParameters.push_back(c);
  }
}





Function* Function_multiply::duplicate()
{
  try
  {
    return (Function*)new Function_multiply(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
