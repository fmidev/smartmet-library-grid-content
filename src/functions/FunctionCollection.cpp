#include "FunctionCollection.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{

FunctionCollection::FunctionCollection()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





FunctionCollection::~FunctionCollection()
{
  try
  {
    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      delete it->second;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::addFunction(const char *functionName,FunctionPtr function)
{
  try
  {
    std::string fname = functionName;
    addFunction(fname,function);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::addFunction(std::string& functionName,FunctionPtr function)
{
  try
  {
    Function *f = getFunction(functionName);
    if (f != NULL)
    {
      Spine::Exception exception(BCP, "The function is already in the collection!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    mFunctionList.insert(std::pair<std::string,FunctionPtr>(toLowerString(functionName),function));
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





FunctionPtr FunctionCollection::getFunction(std::string& functionName)
{
  try
  {
    auto it = mFunctionList.find(toLowerString(functionName));

    if (it != mFunctionList.end())
      return it->second;

    return NULL;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float FunctionCollection::executeFunctionType1(std::string& functionName,std::vector<float>& parameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    return function->executeFunctionType1(parameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





double FunctionCollection::executeFunctionType1(std::string& functionName,std::vector<double>& parameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    return function->executeFunctionType1(parameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::executeFunctionType2(std::string& functionName,std::vector<double>& inOutParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType2(inOutParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::executeFunctionType2(std::string& functionName,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType2(inParameters,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::executeFunctionType2(std::string& functionName,std::vector<float>& inOutParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType2(inOutParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void FunctionCollection::executeFunctionType2(std::string& functionName,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType2(inParameters,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void FunctionCollection::executeFunctionType3(std::string& functionName,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType3(inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::executeFunctionType3(std::string& functionName,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == NULL)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionType3(inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void FunctionCollection::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    for (auto it = mFunctionList.begin(); it != mFunctionList.end(); ++it)
    {
      stream << space(level) << "FunctionCollection\n";
      stream << space(level+1) << it->first << "\n";
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}



}  // namespace Functions
}  // namespace SmartMet
