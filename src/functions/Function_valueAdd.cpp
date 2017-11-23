#include "Function_valueAdd.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_valueAdd::Function_valueAdd(double valueToAdd)
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





Function_valueAdd::Function_valueAdd(const Function_valueAdd& function)
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




Function_valueAdd::~Function_valueAdd()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function_valueAdd::executeFunctionType1(std::vector<float>& parameters)
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





double Function_valueAdd::executeFunctionType1(std::vector<double>& parameters)
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





void Function_valueAdd::executeFunctionType2(std::vector<double>& inOutParameters)
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





void Function_valueAdd::executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters)
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





void Function_valueAdd::executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters)
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





void Function_valueAdd::executeFunctionType2(std::vector<float>& inOutParameters)
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





Function* Function_valueAdd::duplicate()
{
  try
  {
    return (Function*)new Function_valueAdd(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace Functions
}  // namespace SmartMet
