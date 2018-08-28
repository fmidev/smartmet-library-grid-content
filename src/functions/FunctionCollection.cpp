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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::addFunction(std::string& functionName,FunctionPtr function)
{
  try
  {
    Function *f = getFunction(functionName);
    if (f != nullptr)
    {
      Spine::Exception exception(BCP, "The function is already in the collection!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    mFunctionList.insert(std::pair<std::string,FunctionPtr>(toLowerString(functionName),function));
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





FunctionPtr FunctionCollection::getFunction(std::string& functionName)
{
  try
  {
    auto it = mFunctionList.find(toLowerString(functionName));

    if (it != mFunctionList.end())
      return it->second;

    return nullptr;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





float FunctionCollection::executeFunctionCall1(std::string& functionName,std::vector<float>& parameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    return function->executeFunctionCall1(parameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





double FunctionCollection::executeFunctionCall1(std::string& functionName,std::vector<double>& parameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    return function->executeFunctionCall1(parameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<double>& inOutParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall2(columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall2(columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<float>& inOutParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall2(columns,rows,inOutParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




void FunctionCollection::executeFunctionCall2(std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall2(columns,rows,inParameters,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




void FunctionCollection::executeFunctionCall3(std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall3(std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall3(columns,rows,inParameters1,inParameters2,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall4(std::string& functionName,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void FunctionCollection::executeFunctionCall4(std::string& functionName,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    auto function = getFunction(functionName);
    if (function == nullptr)
    {
      Spine::Exception exception(BCP, "Function not found!");
      exception.addParameter("Function name",functionName);
      throw exception;
    }

    function->executeFunctionCall4(columns,rows,inParameters1,inParameters2,angles,outParameters);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}



}  // namespace Functions
}  // namespace SmartMet
