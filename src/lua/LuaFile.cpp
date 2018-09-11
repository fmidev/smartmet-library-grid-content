#include "LuaFile.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Lua
{

extern "C"
{
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}



LuaFile::LuaFile()
{
  try
  {
    mLuaState = nullptr;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", nullptr);
  }
}





LuaFile::LuaFile(const std::string& filename)
{
  try
  {
    mFilename = filename;
    mLuaState = nullptr;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", nullptr);
  }
}





LuaFile::LuaFile(const LuaFile& luaFile)
{
  try
  {
    mFilename = luaFile.mFilename;
    mFunctions = luaFile.mFunctions;
    mLuaState = luaFile.mLuaState;
    mLastModified = luaFile.mLastModified;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Copy constructor failed!", nullptr);
  }
}





LuaFile::~LuaFile()
{
  try
  {
    if (mLuaState != nullptr)
      lua_close((lua_State*)mLuaState);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Destructor failed!", nullptr);
  }
}





void LuaFile::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    loadFile();
    for (int t=1; t<10; t++)
      loadFunctionList(t);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Initialization failed!", nullptr);
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
    throw Spine::Exception(BCP, "Initialization failed!", nullptr);
  }
}





bool LuaFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
    {
      if (mLuaState != nullptr)
      {
        lua_close((lua_State*)mLuaState);
        mLuaState = nullptr;
      }

      loadFile();
      for (int t=1; t<10; t++)
        loadFunctionList(t);

      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Update check failed!", nullptr);
  }
}





uint LuaFile::getFunction(const std::string& functionName,std::string& function)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

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
    throw Spine::Exception(BCP, "LUA function searching failed!", nullptr);
  }
}





float LuaFile::executeFunctionCall1(std::string& function,std::vector<float>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 1)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        Spine::Exception exception(BCP, "LUA function call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





double LuaFile::executeFunctionCall1(std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 1)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        Spine::Exception exception(BCP, "LUA function call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 2)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen = inParameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,inParameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, LUA_MULTRET, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(inParameters.size());

      for (int t=1; t<=pLen; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        double a = lua_tonumber(L, -1);
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }

      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inOutParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 2)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen = inOutParameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,inOutParameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, LUA_MULTRET, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      for (int t=1; t<=pLen; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        double a = lua_tonumber(L, -1);
        inOutParameters[t-1] = a;
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }

      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 2)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen = inParameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,inParameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, LUA_MULTRET, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      outParameters.reserve(inParameters.size());

      for (int t=1; t<=pLen; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        float a = lua_tonumber(L, -1);
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }

      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inOutParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 2)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen = inOutParameters.size();

    lua_getglobal(L,a->second.mFunctionName.c_str());
    //lua_pushinteger(L,pLen);
    lua_pushinteger(L,columns);
    lua_pushinteger(L,rows);

    lua_newtable(L);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber(L,inOutParameters[i]);
      lua_rawseti(L,-2,i + 1);
    }

    int res = lua_pcall(L, 3, LUA_MULTRET, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring(L, -1));
      lua_pop(L, 1);
      throw exception;
    }
    else
    {
      for (int t=1; t<=pLen; t++)
      {
        lua_pushnumber(L,t);
        lua_gettable(L, -2);
        float a = lua_tonumber(L, -1);
        inOutParameters[t-1] = a;
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }

      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 3)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();

    if (pLen1 != pLen2)
    {
      Spine::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",std::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",std::to_string(pLen2));
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

    int res = lua_pcall(L, 4, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 3)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();

    if (pLen1 != pLen2)
    {
      Spine::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",std::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",std::to_string(pLen2));
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

    int res = lua_pcall(L, 4, LUA_MULTRET, 0);

    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 4)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();
    int aLen = angles.size();

    if (pLen1 != pLen2)
    {
      Spine::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",std::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",std::to_string(pLen2));
      throw exception;
    }

    if (pLen1 != aLen)
    {
      Spine::Exception exception(BCP, "There should be as many angles as grid values!");
      exception.addParameter("NumOfValues",std::to_string(pLen1));
      exception.addParameter("NumOfAngles",std::to_string(aLen));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 4)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen1 = inParameters1.size();
    int pLen2 = inParameters2.size();
    int aLen = angles.size();

    if (pLen1 != pLen2)
    {
      Spine::Exception exception(BCP, "Input parameters should have the same number of values!");
      exception.addParameter("NumOfValues(inParameters1)",std::to_string(pLen1));
      exception.addParameter("NumOfValues(inParameters2)",std::to_string(pLen2));
      throw exception;
    }
    if (pLen1 != aLen)
    {
      Spine::Exception exception(BCP, "There should be as many angles as grid values!");
      exception.addParameter("NumOfValues",std::to_string(pLen1));
      exception.addParameter("NumOfAngles",std::to_string(aLen));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        outParameters.push_back(a);
        //printf("%d => %f\n",t,a);
        lua_pop(L, 1);
      }
      lua_pop(L, 1);  // pop table from the stack
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall5(std::string& function,std::string language,std::vector<float>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 5)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        Spine::Exception exception(BCP, "LUA function call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall5(std::string& function,std::string language,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 5)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        Spine::Exception exception(BCP, "LUA function call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall6(std::string& function,std::vector<std::string>& params)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 6)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





std::string LuaFile::executeFunctionCall6(
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
    std::vector<std::string> params;
    params.push_back(producerName);
    params.push_back(parameterName);
    params.push_back(std::to_string(parameterKeyType));
    params.push_back(parameterKey);
    params.push_back(std::to_string(parameterLevelIdType));
    params.push_back(std::to_string(parameterLevelId));
    params.push_back(std::to_string(parameterLevel));
    params.push_back(std::to_string(forecastType));
    params.push_back(std::to_string(forecastNumber));
    params.push_back(std::to_string(interpolationMethod));

    return executeFunctionCall6(function,params);


#if 0
    AutoThreadLock lock(&mThreadLock);

    lua_State *L = (lua_State*)mLuaState;

    auto a = mFunctions.find(toLowerString(function));
    if (a == mFunctions.end())
    {
      Spine::Exception exception(BCP, "Unknown function!");
      exception.addParameter("Function",function);
      throw exception;
    }

    if (a->second.mType != 7)
    {
      Spine::Exception exception(BCP, "Invalid function type!");
      exception.addDetail("You should probably use different 'executeFunction' with this LUA function.");
      exception.addDetail("That's because the current LUA function does not support the same parameters");
      exception.addDetail("or the return values that this 'executeFunction' is using.");
      exception.addParameter("LUA File",mFilename);
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
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
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", nullptr);
  }
}





void LuaFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LuaFile\n";
    stream << space(level) << "- mFilename  = " << mFilename << "\n";

    for (auto it = mFunctions.begin(); it != mFunctions.end(); ++it)
      it->second.print(stream,level+2,optionFlags);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void LuaFile::loadFile()
{
  try
  {
    if (mLuaState != nullptr)
    {
      lua_close((lua_State*)mLuaState);
      mLuaState = nullptr;
    }

    mLuaState = (lua_State*)luaL_newstate();
    luaL_openlibs((lua_State*)mLuaState);

    if (luaL_dofile((lua_State*)mLuaState,mFilename.c_str()) != 0)
    {
      Spine::Exception exception(BCP, "Cannot load a LUA file!");
      exception.addParameter("Filename",mFilename);
      exception.addParameter("Lua message",lua_tostring((lua_State*)mLuaState, -1));
      lua_pop((lua_State*)mLuaState, 1);
      throw exception;
    }

    mLastModified = getFileModificationTime(mFilename.c_str());
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "File loading failed!", nullptr);
  }
}




void LuaFile::loadFunctionList(uint type)
{
  try
  {
    lua_State *L = (lua_State*)mLuaState;

    lua_getglobal(L,"getFunctionNames");
    lua_pushinteger(L,type);

    int res = lua_pcall(L, 1, 1, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
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
        if (cstr[t] != '\0'  &&  cstr[t] != ','  &&  c < 100)
        {
          buf[c] = cstr[t];
          c++;
        }
        else
        {
          buf[c] = '\0';
          if (c > 0)
          {
            //printf("Function : [%s]\n",buf);
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
    throw Spine::Exception(BCP, "LUA function list loading failed!", nullptr);
  }
}


}  // namespace Lua
}  // namespace SmartMet
