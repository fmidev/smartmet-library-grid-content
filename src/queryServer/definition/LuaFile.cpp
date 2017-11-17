#include "LuaFile.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace QueryServer
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
    mLuaState = NULL;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", NULL);
  }
}





LuaFile::LuaFile(std::string filename)
{
  try
  {
    mFilename = filename;
    mLuaState = NULL;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Constructor failed!", NULL);
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
    throw Spine::Exception(BCP, "Copy constructor failed!", NULL);
  }
}





LuaFile::~LuaFile()
{
  try
  {
    if (mLuaState != NULL)
      lua_close((lua_State*)mLuaState);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Destructor failed!", NULL);
  }
}





void LuaFile::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    loadFile();
    loadFunctionList(1);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Initialization failed!", NULL);
  }
}





void LuaFile::init(std::string filename)
{
  try
  {
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Initialization failed!", NULL);
  }
}




void LuaFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(0))
    {
      if (mLuaState != NULL)
      {
        lua_close((lua_State*)mLuaState);
        mLuaState = NULL;
      }

      loadFile();
      loadFunctionList(1);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Update check failed!", NULL);
  }
}





bool LuaFile::getFunction(std::string& functionName,std::string& function)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    auto a = mFunctions.find(toLowerString(functionName));
    if (a != mFunctions.end())
    {
      function = a->second.mFunctionName;
      return true;
    }
    return false;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function searching failed!", NULL);
  }
}





double LuaFile::executeFunction(std::string& function,std::vector<double>& parameters)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

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
      exception.addDetail("or the return values that this 'excecuteFunction' is using.");
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA Function Type",std::to_string(a->second.mType));
      throw exception;
    }

    int pLen = (int)parameters.size();

    lua_getglobal((lua_State*)mLuaState,a->second.mFunctionName.c_str());
    //lua_pushstring(mLuaState,a->mFunctionName.c_str());
    lua_pushinteger((lua_State*)mLuaState,pLen);

    lua_newtable((lua_State*)mLuaState);
    for (int i = 0; i < pLen; i++)
    {
      lua_pushnumber((lua_State*)mLuaState,parameters[i]);
      lua_rawseti((lua_State*)mLuaState,-2,i + 1);
    }

    int res = lua_pcall((lua_State*)mLuaState, 2, 2, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("LUA Function",function);
      exception.addParameter("LUA message",lua_tostring((lua_State*)mLuaState, -1));
      lua_pop((lua_State*)mLuaState, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring((lua_State*)mLuaState, -1, &len);
      std::string message(cstr, len);
      lua_pop((lua_State*)mLuaState, 1);  /* pop returned value */

      double value = lua_tonumber((lua_State*)mLuaState,-1);
      lua_pop((lua_State*)mLuaState, 1);  /* pop returned value */

      if (message != "OK")
      {
        Spine::Exception exception(BCP, "LUA function call returns an error!");
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
    throw Spine::Exception(BCP, "LUA function execution failed!", NULL);
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
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void LuaFile::loadFile()
{
  try
  {
    if (mLuaState != NULL)
    {
      lua_close((lua_State*)mLuaState);
      mLuaState = NULL;
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
    throw Spine::Exception(BCP, "File loading failed!", NULL);
  }
}




void LuaFile::loadFunctionList(uint type)
{
  try
  {
    lua_getglobal((lua_State*)mLuaState,"getFunctionNames");
    lua_pushinteger((lua_State*)mLuaState,type);

    int res = lua_pcall((lua_State*)mLuaState, 1, 1, 0);
    if (res != 0)
    {
      // LUA ERROR
      Spine::Exception exception(BCP, "LUA call returns an error!");
      exception.addParameter("Lua message",lua_tostring((lua_State*)mLuaState, -1));
      lua_pop((lua_State*)mLuaState, 1);
      throw exception;
    }
    else
    {
      size_t len = 0;
      const char* cstr = lua_tolstring((lua_State*)mLuaState, -1, &len);

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
            printf("Function : [%s]\n",buf);
            mFunctions.insert(std::pair<std::string,LuaFunction>(toLowerString(std::string(buf)),LuaFunction(buf,type)));
          }
          c = 0;
        }
      }
      lua_pop((lua_State*)mLuaState, 1);  /* pop returned value */
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "LUA function list loading failed!", NULL);
  }
}


}  // namespace QueryServer
}  // namespace SmartMet
