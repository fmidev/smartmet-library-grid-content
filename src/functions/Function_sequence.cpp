#include "Function_sequence.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_sequence::Function_sequence()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function_sequence::Function_sequence(const Function_sequence& function)
:Function(function)
{
  try
  {
    for (auto it = function.mFunctionList.begin(); it != function.mFunctionList.end(); ++it)
    {
      mFunctionList.push_back((*it)->duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function_sequence::~Function_sequence()
{
  try
  {
    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      delete *it;
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void Function_sequence::addFunction(Function *function)
{
  try
  {
    mFunctionList.push_back(function);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





float Function_sequence::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 1)
      throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");

    float val = parameters[0];

    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      std::vector<float> params;
      params.push_back(val);

      val = (*it)->executeFunctionCall1(params);
    }
    return val;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_sequence::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 1)
      throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");

    double val = parameters[0];

    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      std::vector<double> params;
      params.push_back(val);

      val = (*it)->executeFunctionCall1(params);
    }
    return val;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function* Function_sequence::duplicate()
{
  try
  {
    return (Function*)new Function_sequence(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}








}  // namespace Functions
}  // namespace SmartMet
