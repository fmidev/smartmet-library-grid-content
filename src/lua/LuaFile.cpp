#include "LuaFile.h"
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/StringConversion.h>



namespace SmartMet
{
namespace Lua
{

extern "C"
{
  // Standard lua header files
  //#include <lua.h>
  //#include <lauxlib.h>
  //#include <lualib.h>

  // Lua header files when using luajit compiler
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
  #include <luajit.h>
}



LuaFile::LuaFile()
{
  try
  {
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      mLuaState[t] = nullptr;
      mStateKey[t] = 0;
    }

    mLastModified = 0;
    mStateKeyCounter = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Constructor failed!", nullptr);
  }
}





LuaFile::LuaFile(const std::string& filename)
{
  try
  {
    mFilename = filename;
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      mLuaState[t] = nullptr;
      mStateKey[t] = 0;
    }

    mLastModified = 0;
    mStateKeyCounter = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Constructor failed!", nullptr);
  }
}





LuaFile::LuaFile(const LuaFile& luaFile)
{
  try
  {
    mFilename = luaFile.mFilename;
    mFunctions = luaFile.mFunctions;
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      mLuaState[t] = nullptr;
      mStateKey[t] = 0;
    }

    mLastModified = luaFile.mLastModified;
    mStateKeyCounter = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Copy constructor failed!", nullptr);
  }
}





LuaFile::~LuaFile()
{
  try
  {
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      if (mLuaState[t] != nullptr)
        lua_close((lua_State*)mLuaState[t]);
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void LuaFile::init()
{
  try
  {
    AutoWriteLock lock(&mModificationLock);

    loadFile();
    mFunctions.clear();

    for (int t=1; t<10; t++)
      loadFunctionList(t);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}





void LuaFile::init(const std::string& filename)
{
  try
  {
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}





void* LuaFile::getLuaState(ulonglong& key)
{
  try
  {
    AutoWriteLock lock(&mStateModificationLock);
    while (true)
    {
      for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
      {
        if (mStateKey[t] == 0)
        {
          mStateKeyCounter++;
          key = mStateKeyCounter;
          mStateKey[t] = key;
          return mLuaState[t];
        }
      }
      time_usleep(0,100);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}





void LuaFile::releaseLuaState(ulonglong key)
{
  try
  {
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      if (mStateKey[t] == key)
      {
        mStateKey[t] = 0;
        return;
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Initialization failed!", nullptr);
  }
}




bool LuaFile::checkUpdates()
{
  try
  {
    time_t tt = getFileModificationTime(mFilename.c_str());
    if (tt == mLastModified  ||  (tt+3) >= time(nullptr))
      return false;

    AutoWriteLock lock(&mModificationLock);

    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
    {
      AutoWriteLock lock(&mStateModificationLock);

      // Waiting until all states are free.
      bool ind = false;
      while (!ind)
      {
        uint count = 0;
        for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
        {
          if (mStateKey[t] == 0)
            count++;
        }

        if (count == NUM_OF_LUA_HANDLES)
          ind = true;
        else
          time_usleep(0,100);
      }


      for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
      {
        if (mLuaState[t] != nullptr)
        {
          lua_close((lua_State*)mLuaState[t]);
          mLuaState[t] = nullptr;
        }
      }

      loadFile();
      mFunctions.clear();

      for (int t=1; t<10; t++)
        loadFunctionList(t);

      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Update check failed!", nullptr);
  }
}





uint LuaFile::getFunction(const std::string& functionName,std::string& function)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    auto a = mFunctions.find(toLowerString(functionName));
    if (a != mFunctions.end())
    {
      function = a->second.mFunctionName;
      return a->second.mType;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function searching failed!", nullptr);
  }
}





float LuaFile::executeFunctionCall1(const std::string& function,std::vector<float>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 1)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    int pLen = parameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());

    //lua_pushstring(mLuaState,a->mFunctionName.c_str());
    lua_pushinteger(L,pLen);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,parameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 2, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string message(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      float value = lua_tonumber(L,-1);
      lua_pop(L, 1);  /* pop returned value */

      if (message != "OK")
      {
        Fmi::Exception exception(BCP, "LUA function call returns an error!");
        exception.addParameter("LUA File",mFilename);
        exception.addParameter("LUA Function",function);
        exception.addParameter("Error Message",message.c_str());
        throw exception;
      }
      return value;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





double LuaFile::executeFunctionCall1(const std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 1)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    int pLen = parameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());
    lua_pushinteger(L,pLen);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,parameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 2, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string message(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      double value = lua_tonumber(L,-1);
      lua_pop(L, 1);  /* pop returned value */

      if (message != "OK")
      {
        Fmi::Exception exception(BCP, "LUA function call returns an error!");
        exception.addParameter("LUA File",mFilename);
        exception.addParameter("LUA Function",function);
        exception.addParameter("Error Message",message.c_str());
        throw exception;
      }
      return value;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





double LuaFile::executeFunctionCall2(const std::string& function,const char *str)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 2)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    lua_getglobal(L,a->second.mFunctionName.c_str());
    lua_pushstring(L,str);

    int res = lua_pcall(L, 1, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string message(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      double value = lua_tonumber(L,-1);
      lua_pop(L, 1);  /* pop returned value */

      if (message != "OK")
      {
        Fmi::Exception exception(BCP, "LUA function call returns an error!");
        exception.addParameter("LUA File",mFilename);
        exception.addParameter("LUA Function",function);
        exception.addParameter("Error Message",message.c_str());
        throw exception;
      }
      return value;
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}




void LuaFile::executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 4)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();
    int aLen = angles.size();

    if (pLen1 != pLen2)
    {
      Fmi::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",Fmi::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",Fmi::to_string(pLen2));
      throw exception;
    }

    if (pLen1 != aLen)
    {
      Fmi::Exception exception(BCP, "There should be as many angles as grid values!");
      exception.addParameter("NumOfValues",Fmi::to_string(pLen1));
      exception.addParameter("NumOfAngles",Fmi::to_string(aLen));
      throw exception;
    }

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen1; i++)
    {
      lua_pushnumber(L,inParameters1[i]);
      lua_rawseti(L,-2,i + 1);
    }

    lua_newtable(L);
    for (int i = 0; i < pLen1; i++)
    {
      lua_pushnumber(L,inParameters2[i]);
      lua_rawseti(L,-2,i + 1);
    }

    lua_newtable(L);
    for (int i = 0; i < aLen; i++)
    {
      lua_pushnumber(L,angles[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 5, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(pLen1);

      for (int t=1; t<=pLen1; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        float a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 4)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();
    int aLen = angles.size();

    if (pLen1 != pLen2)
    {
      Fmi::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",Fmi::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",Fmi::to_string(pLen2));
      throw exception;
    }
    if (pLen1 != aLen)
    {
      Fmi::Exception exception(BCP, "There should be as many angles as grid values!");
      exception.addParameter("NumOfValues",Fmi::to_string(pLen1));
      exception.addParameter("NumOfAngles",Fmi::to_string(aLen));
      throw exception;
    }
    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen1; i++)
    {
      lua_pushnumber(L,inParameters1[i]);
      lua_rawseti(L,-2,i + 1);
    }

    lua_newtable(L);
    for (int i = 0; i < pLen1; i++)
    {
      lua_pushnumber(L,inParameters2[i]);
      lua_rawseti(L,-2,i + 1);
    }

    lua_newtable(L);
    for (int i = 0; i < aLen; i++)
    {
      lua_pushnumber(L,angles[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 5, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(pLen1);

      for (int t=1; t<=pLen1; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        double a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall5(const std::string& function,const std::string& language,std::vector<float>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 5)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    int pLen = parameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());

    lua_pushstring(L,language.c_str());
    lua_pushinteger(L,pLen);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,parameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string message(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      const char* vstr = lua_tolstring(L, -1, &len);
      std::string value(vstr, len);
      lua_pop(L, 1);  /* pop returned value */

      if (message != "OK")
      {
        Fmi::Exception exception(BCP, "LUA function call returns an error!");
        exception.addParameter("LUA File",mFilename);
        exception.addParameter("LUA Function",function);
        exception.addParameter("Error Message",message.c_str());
        throw exception;
      }

      return value;
    }
    return std::string("");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall5(const std::string& function,const std::string& language,std::vector<double>& parameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 5)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    int pLen = parameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());

    lua_pushstring(L,language.c_str());
    lua_pushinteger(L,pLen);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,parameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string message(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      const char* vstr = lua_tolstring(L, -1, &len);
      std::string value(vstr, len);
      lua_pop(L, 1);  /* pop returned value */

      if (message != "OK")
      {
        Fmi::Exception exception(BCP, "LUA function call returns an error!");
        exception.addParameter("LUA File",mFilename);
        exception.addParameter("LUA Function",function);
        exception.addParameter("Error Message",message.c_str());
        throw exception;
      }

      return value;
    }
    return std::string("");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall6(const std::string& function,std::vector<std::string>& params)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 6)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    lua_getglobal(L,a->second.mFunctionName.c_str());

    //lua_pushstring(mLuaState,a->mFunctionName.c_str());
    //lua_pushinteger(L,pLen);

    int len = params.size();
    lua_pushinteger(L,len);

    lua_newtable(L);
    for (int i = 0; i < len; i++)
    {
      lua_pushstring(L,params[i].c_str());
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 2, 1, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string value(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      return value;
    }
    return std::string("");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall6(
                  const std::string& function,
                  const std::string& producerName,
                  const std::string& parameterName,
                  T::ParamKeyType parameterKeyType,
                  const std::string& parameterKey,
                  short parameterLevelId,
                  int parameterLevel,
                  short forecastType,
                  short forecastNumber,
                  short interpolationMethod)
{
  try
  {
    std::vector<std::string> params;
    params.emplace_back(producerName);
    params.emplace_back(parameterName);
    params.emplace_back(Fmi::to_string(parameterKeyType));
    params.emplace_back(parameterKey);
    params.emplace_back(Fmi::to_string(parameterLevelId));
    params.emplace_back(Fmi::to_string(parameterLevel));
    params.emplace_back(Fmi::to_string(forecastType));
    params.emplace_back(Fmi::to_string(forecastNumber));
    params.emplace_back(Fmi::to_string(interpolationMethod));

    return executeFunctionCall6(function,params);


#if 0
    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 7)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }


    lua_getglobal(L,a->second.mFunctionName.c_str());

    //lua_pushstring(mLuaState,a->mFunctionName.c_str());
    //lua_pushinteger(L,pLen);

    lua_pushstring(L,producerName.c_str());
    lua_pushstring(L,parameterName.c_str());
    lua_pushinteger(L,parameterKeyType);
    lua_pushstring(L,parameterKey.c_str());
    lua_pushinteger(L,parameterLevelIdType);
    lua_pushinteger(L,parameterLevelId);
    lua_pushinteger(L,parameterLevel);
    lua_pushinteger(L,forecastType);
    lua_pushinteger(L,forecastNumber);
    lua_pushinteger(L,interpolationMethod);

    int res = lua_pcall(L, 10, 1, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);
      std::string value(cstr, len);
      lua_pop(L, 1);  /* pop returned value */

      return value;
    }
#endif
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall8(const std::string& function,const char *subfunction,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 8)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    uint sz = inParameters.size();
    uint gridSize = columns*rows;

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushstring(L,subfunction);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);
    lua_pushinteger(L,sz);

    lua_newtable(L);
    uint c = 0;
    for (uint i = 0; i < gridSize; i++)
    {
      for (uint t=0; t<sz; t++)
      {
        if (i < inParameters[t].size())
          lua_pushnumber(L,inParameters[t][i]);
        else
          lua_pushnumber(L,ParamValueMissing);

        lua_rawseti(L,-2,c + 1);
        c++;
      }
    }

    lua_newtable(L);
    uint esz = extParameters.size();
    for (uint t=0; t<esz; t++)
    {
      lua_pushnumber(L,extParameters[t]);
      lua_rawseti(L,-2,t + 1);
    }

    int res = lua_pcall(L, 6, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(gridSize);

      for (uint t=1; t<=gridSize; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        float a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall8(const std::string& function,const char *subfunction,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 8)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    uint sz = inParameters.size();
    uint gridSize = columns*rows;

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushstring(L,subfunction);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);
    lua_pushinteger(L,sz);

    lua_newtable(L);
    uint c = 0;
    for (uint i = 0; i < gridSize; i++)
    {
      for (uint t=0; t<sz; t++)
      {
        if (i < inParameters[t].size())
          lua_pushnumber(L,inParameters[t][i]);
        else
          lua_pushnumber(L,ParamValueMissing);

        lua_rawseti(L,-2,c + 1);
        c++;
      }
    }

    lua_newtable(L);
    uint esz = extParameters.size();
    for (uint t=0; t<esz; t++)
    {
      lua_pushnumber(L,extParameters[t]);
      lua_rawseti(L,-2,t + 1);
    }

    int res = lua_pcall(L, 6, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(gridSize);

      for (uint t=1; t<=gridSize; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        double a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 9)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    uint sz = inParameters.size();
    uint gridSize = columns*rows;

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);
    lua_pushinteger(L,sz);

    lua_newtable(L);
    uint c = 0;
    for (uint i = 0; i < gridSize; i++)
    {
      for (uint t=0; t<sz; t++)
      {
        if (i < inParameters[t].size())
          lua_pushnumber(L,inParameters[t][i]);
        else
          lua_pushnumber(L,ParamValueMissing);

        lua_rawseti(L,-2,c + 1);
        c++;
      }
    }

    lua_newtable(L);
    uint esz = extParameters.size();
    for (uint t=0; t<esz; t++)
    {
      lua_pushnumber(L,extParameters[t]);
      lua_rawseti(L,-2,t + 1);
    }

    int res = lua_pcall(L, 5, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(gridSize);

      for (uint t=1; t<=gridSize; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        float a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    LuaHandle luaHandle(this);
    lua_State *L = (lua_State*)luaHandle.getState();

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Fmi::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 9)
    {
      Fmi::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",Fmi::to_string(a->second.mType));
      throw exception;
    }

    uint sz = inParameters.size();
    uint gridSize = columns*rows;

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);
    lua_pushinteger(L,sz);

    lua_newtable(L);
    uint c = 0;
    for (uint i = 0; i < gridSize; i++)
    {
      for (uint t=0; t<sz; t++)
      {
        if (i < inParameters[t].size())
          lua_pushnumber(L,inParameters[t][i]);
        else
          lua_pushnumber(L,ParamValueMissing);

        lua_rawseti(L,-2,c + 1);
        c++;
      }
    }

    lua_newtable(L);
    uint esz = extParameters.size();
    for (uint t=0; t<esz; t++)
    {
      lua_pushnumber(L,extParameters[t]);
      lua_rawseti(L,-2,t + 1);
    }

    int res = lua_pcall(L, 5, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(gridSize);

      for (uint t=1; t<=gridSize; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        double a = lua_tonumber(L, -1);
        outParameters.emplace_back(a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}




void LuaFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    AutoReadLock lock(&mModificationLock);

    stream << space(level) << "LuaFile\n";
    stream << space(level) << "- mFilename  = " << mFilename << "\n";

    for (auto it = mFunctions.begin(); it != mFunctions.end(); ++it)
      it->second.print(stream,level+2,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void LuaFile::loadFile()
{
  try
  {
    for (uint t=0; t<NUM_OF_LUA_HANDLES; t++)
    {
      if (mLuaState[t] != nullptr)
      {
        lua_close((lua_State*)mLuaState[t]);
        mLuaState[t] = nullptr;
      }

      mLuaState[t] = luaL_newstate();
      luaL_openlibs((lua_State*)mLuaState[t]);

      if (luaL_dofile((lua_State*)mLuaState[t],mFilename.c_str()) != 0)
      {
        Fmi::Exception exception(BCP, "Cannot load a LUA file!");
        exception.addParameter("Filename",mFilename);
        exception.addParameter("Lua message",lua_tostring((lua_State*)mLuaState[t], -1));
        lua_pop((lua_State*)mLuaState[t], 1);
        throw exception;
      }
    }

    mLastModified = getFileModificationTime(mFilename.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "File loading failed!", nullptr);
  }
}




void LuaFile::loadFunctionList(uint type)
{
  try
  {
    lua_State *L =(lua_State*)mLuaState[0];

    lua_getglobal(L,"getFunctionNames");
    lua_pushinteger(L,type);

    int res = lua_pcall(L, 1, 1, 0);
    if (res != 0)
    {
      // LUA ERROR
      Fmi::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("Lua message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring(L, -1, &len);

      char buf[100];
      uint c = 0;
      for (size_t t = 0; t<= len; t++)
      {
        if (cstr[t] != '\0'  &&  cstr[t] != ','  &&  c < 99)
        {
          buf[c] = cstr[t];
          c++;
        }
        else
        {
          buf[c] = '\0';
          if (c > 0)
          {
            mFunctions.insert(std::pair<std::string,LuaFunction>(toLowerString(std::string(buf)),LuaFunction(buf,type)));
          }
          c = 0;
        }
      }
      lua_pop(L, 1);  /* pop returned value */
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "LUA function list loading failed!", nullptr);
  }
}


}  // namespace Lua
}  // namespace SmartMet
