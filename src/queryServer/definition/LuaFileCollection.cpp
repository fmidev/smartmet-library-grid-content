#include "LuaFileCollection.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
{

LuaFileCollection::LuaFileCollection()
{
  try
  {
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




void LuaFileCollection::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      it->checkUpdates();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Update check failed!", NULL);
  }
}





bool LuaFileCollection::getFunction(std::string& functionName,std::string& function)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      if (it->getFunction(functionName,function))
        return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function searching failed!", NULL);
  }
}





double LuaFileCollection::executeFunction(std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    for (auto it = mLuaFileList.begin(); it != mLuaFileList.end(); ++it)
    {
      std::string functionName;
      if (it->getFunction(function,functionName))
      {
        return it->executeFunction(functionName,parameters);
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






}  // namespace QueryServer
}  // namespace SmartMet
