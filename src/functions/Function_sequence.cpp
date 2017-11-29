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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function_sequence::executeFunctionType1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 1)
      throw Spine::Exception(BCP, "The parameters vector should contain only one value!");

    double val = parameters[0];

    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      std::vector<float> params;
      params.push_back(val);

      val = (*it)->executeFunctionType1(params);
    }
    return val;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





double Function_sequence::executeFunctionType1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 1)
      throw Spine::Exception(BCP, "The parameters vector should contain only one value!");

    double val = parameters[0];

    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      std::vector<double> params;
      params.push_back(val);

      val = (*it)->executeFunctionType1(params);
    }
    return val;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void Function_sequence::executeFunctionType2(std::vector<double>& inOutParameters)
{
  try
  {
    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      (*it)->executeFunctionType2(inOutParameters);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_sequence::executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    uint len = (uint)mFunctionList.size();
    if (len == 1)
    {
      mFunctionList[0]->executeFunctionType2(inParameters,outParameters);
      return;
    }

    std::vector<double> tmpOut1;
    std::vector<double> tmpOut2;

    for (uint t=0; t<len; t++)
    {
      if (t == 0)
        mFunctionList[t]->executeFunctionType2(inParameters,tmpOut1);
      else
      if (t == (len-1))
      {
        if (tmpOut1.size() > 0)
          mFunctionList[t]->executeFunctionType2(tmpOut1,outParameters);
        else
          mFunctionList[t]->executeFunctionType2(tmpOut2,outParameters);
      }
      else
      {
        if (tmpOut1.size() > 0)
        {
          mFunctionList[t]->executeFunctionType2(tmpOut1,tmpOut2);
          tmpOut1.clear();
        }
        else
        {
          mFunctionList[t]->executeFunctionType2(tmpOut2,tmpOut1);
          tmpOut2.clear();
        }
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_sequence::executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    uint len = (uint)mFunctionList.size();
    if (len == 1)
    {
      mFunctionList[0]->executeFunctionType2(inParameters,outParameters);
      return;
    }

    std::vector<float> tmpOut1;
    std::vector<float> tmpOut2;

    for (uint t=0; t<len; t++)
    {
      if (t == 0)
        mFunctionList[t]->executeFunctionType2(inParameters,tmpOut1);
      else
      if (t == (len-1))
      {
        if (tmpOut1.size() > 0)
          mFunctionList[t]->executeFunctionType2(tmpOut1,outParameters);
        else
          mFunctionList[t]->executeFunctionType2(tmpOut2,outParameters);
      }
      else
      {
        if (tmpOut1.size() > 0)
        {
          mFunctionList[t]->executeFunctionType2(tmpOut1,tmpOut2);
          tmpOut1.clear();
        }
        else
        {
          mFunctionList[t]->executeFunctionType2(tmpOut2,tmpOut1);
          tmpOut2.clear();
        }
      }
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_sequence::executeFunctionType2(std::vector<float>& inOutParameters)
{
  try
  {
    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      (*it)->executeFunctionType2(inOutParameters);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}








}  // namespace Functions
}  // namespace SmartMet