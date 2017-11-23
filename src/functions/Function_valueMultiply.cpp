#include "Function_valueMultiply.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_valueMultiply::Function_valueMultiply(double multiplyer)
{
  try
  {
    mMultiplyer = multiplyer;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_valueMultiply::Function_valueMultiply(const Function_valueMultiply& function)
:Function(function)
{
  try
  {
    mMultiplyer = function.mMultiplyer;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




Function_valueMultiply::~Function_valueMultiply()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function_valueMultiply::executeFunctionType1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] * mMultiplyer;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





double Function_valueMultiply::executeFunctionType1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
      return parameters[0] * mMultiplyer;

    throw Spine::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_valueMultiply::executeFunctionType2(std::vector<double>& inOutParameters)
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_valueMultiply::executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters)
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_valueMultiply::executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters)
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_valueMultiply::executeFunctionType2(std::vector<float>& inOutParameters)
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function* Function_valueMultiply::duplicate()
{
  try
  {
    return (Function*)new Function_valueMultiply(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace Functions
}  // namespace SmartMet
