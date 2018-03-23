#include "LuaFileCollection.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Lua
{

LuaFileCollection::LuaFileCollection()
{
  try
  {
    mLastCheck = 0;
    mCheckInterval = 10;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", NULL);
  }
}





LuaFileCollection::LuaFileCollection(string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", NULL);
  }
}





LuaFileCollection::LuaFileCollection(const LuaFileCollection& luaFileCollection)
{
  try
  {
    mFilenames = luaFileCollection.mFilenames;
    mLuaFileList = luaFileCollection.mLuaFileList;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Copy constructor failed!", NULL);
  }
}





LuaFileCollection::~LuaFileCollection()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Destructor failed!", NULL);
  }
}





void LuaFileCollection::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mFilenames.begin(); it != mFilenames.end(); ++it)
    {
      mLuaFileList.push_back(LuaFile(*it));
    }

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      it->init();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Initialization failed!", NULL);
  }
}





void LuaFileCollection::init(string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
    init();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Initialization failed!", NULL);
  }
}




void LuaFileCollection::checkUpdates(bool force)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = time(0);
    if (force ||  (tt-mLastCheck) > mCheckInterval)
    {
      for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
      {
        it->checkUpdates();
      }
      mLastCheck = tt;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Update check failed!", NULL);
  }
}





uint LuaFileCollection::getFunction(std::string& functionName,std::string& function)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      uint functionType = it->getFunction(functionName,function);
      if (functionType != 0)
        return functionType;
    }
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function searching failed!", NULL);
  }
}




bool LuaFileCollection::getFunction(std::string& functionName,uint functionType,std::string& function)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      if (it->getFunction(functionName,function) == functionType)
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function searching failed!", NULL);
  }
}





std::string LuaFileCollection::executeFunctionCall5(std::string& function,std::string language,std::vector<float>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 5)
      {
        return it->executeFunctionCall5(functionName,language,parameters);
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





std::string LuaFileCollection::executeFunctionCall5(std::string& function,std::string language,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 5)
      {
        return it->executeFunctionCall5(functionName,language,parameters);
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





float LuaFileCollection::executeFunctionCall1(std::string& function,std::vector<float>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 1)
      {
        return it->executeFunctionCall1(functionName,parameters);
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





double LuaFileCollection::executeFunctionCall1(std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 1)
      {
        return it->executeFunctionCall1(functionName,parameters);
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inOutParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 2)
      {
        it->executeFunctionCall2(functionName,columns,rows,inOutParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 2)
      {
        it->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 2)
      {
        it->executeFunctionCall2(functionName,columns,rows,inParameters,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inOutParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 2)
      {
        it->executeFunctionCall2(functionName,columns,rows,inOutParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 3)
      {
        it->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 3)
      {
        it->executeFunctionCall3(functionName,columns,rows,inParameters1,inParameters2,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 4)
      {
        it->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 4)
      {
        it->executeFunctionCall4(functionName,columns,rows,inParameters1,inParameters2,angles,outParameters);
        return;
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





std::string LuaFileCollection::executeFunctionCall7(
                  std::string& function,
                  std::string& producerName,
                  std::string& parameterName,
                  T::ParamKeyType parameterKeyType,
                  std::string& parameterKey,
                  T::ParamLevelIdType parameterLevelIdType,
                  short parameterLevelId,
                  int parameterLevel,
                  short forecastType,
                  short forecastNumber,
                  short interpolationMethod)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 7)
      {
        return it->executeFunctionCall7(functionName,producerName,
            parameterName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,
            parameterLevel,forecastType,forecastNumber,interpolationMethod);
      }
    }

    Spine::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
  }
}





void LuaFileCollection::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LuaFileCollection\n";

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
      it->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}






}  // namespace Lua
}  // namespace SmartMet
