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
    mCheckInterval = 30;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Constructor failed!", nullptr);
  }
}





LuaFileCollection::LuaFileCollection(const string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Constructor failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Copy constructor failed!", nullptr);
  }
}





LuaFileCollection::~LuaFileCollection()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void LuaFileCollection::init()
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

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
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}





void LuaFileCollection::init(const string_vec& filenames)
{
  try
  {
    mFilenames = filenames;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}




bool LuaFileCollection::checkUpdates(bool force)
{
  try
  {
    bool result = false;
    AutoReadLock lock(&mModificationLock);

    time_t tt = time(nullptr);
    if (force ||  (tt-mLastCheck) > mCheckInterval)
    {

      for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
      {
        if (it->checkUpdates())
          result = true;
      }
      mLastCheck = tt;
    }
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Update check failed!", nullptr);
  }
}





uint LuaFileCollection::getFunction(const std::string& functionName,std::string& function)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

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
    throw Fmi::Exception(BCP, "LUA function searching failed!", nullptr);
  }
}




bool LuaFileCollection::getFunction(const std::string& functionName,const uint functionType,std::string& function)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      if (it->getFunction(functionName,function) == functionType)
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function searching failed!", nullptr);
  }
}





std::string LuaFileCollection::executeFunctionCall5(std::string& function,std::string language,std::vector<float>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 5)
      {
        return it->executeFunctionCall5(functionName,language,parameters);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFileCollection::executeFunctionCall5(std::string& function,std::string language,std::vector<double>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 5)
      {
        return it->executeFunctionCall5(functionName,language,parameters);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





float LuaFileCollection::executeFunctionCall1(std::string& function,std::vector<float>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 1)
      {
        return it->executeFunctionCall1(functionName,parameters);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





double LuaFileCollection::executeFunctionCall1(std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 1)
      {
        return it->executeFunctionCall1(functionName,parameters);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFileCollection::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
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

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFileCollection::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
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

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFileCollection::executeFunctionCall6(std::string& function,std::vector<std::string>& params)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 6)
      {
        return it->executeFunctionCall6(functionName,params);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFileCollection::executeFunctionCall6(
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
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 6)
      {
        return it->executeFunctionCall6(functionName,producerName,
            parameterName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,
            parameterLevel,forecastType,forecastNumber,interpolationMethod);
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}






void LuaFileCollection::executeFunctionCall9(std::string& function,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 9)
      {
        it->executeFunctionCall9(functionName,columns,rows,inParameters,extParameters,outParameters);
        return;
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFileCollection::executeFunctionCall9(std::string& function,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      uint type = it->getFunction(function,functionName);
      if (type == 9)
      {
        it->executeFunctionCall9(functionName,columns,rows,inParameters,extParameters,outParameters);
        return;
      }
    }

    Fmi::Exception exception(BCP, "Unknown LUA function!");
    exception.addParameter("Function",function);
    throw exception;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Lua
}  // namespace SmartMet
