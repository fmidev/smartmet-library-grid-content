#pragma once

#include "LuaFunction.h"
#include <grid-files/common/Coordinate.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include <grid-files/grid/Typedefs.h>

#include <unordered_map>

#define NUM_OF_LUA_HANDLES 20


namespace SmartMet
{
namespace Lua
{

class LuaFile
{
  public:
                      LuaFile();
                      LuaFile(const std::string& filename);
                      LuaFile(const LuaFile& luaFile);
    virtual           ~LuaFile();

    void              init();
    void              init(const std::string& filename);
    bool              checkUpdates();
    uint              getFunction(const std::string& functionName,std::string& function);

    // Type 1: Takes one or several parameters, returns one value
    float             executeFunctionCall1(const std::string& function,std::vector<float>& parameters);
    double            executeFunctionCall1(const std::string& function,std::vector<double>& parameters);

    // Type 4: Takes two data vectors and angle vector, returns one vector
    void              executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    void              executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    std::string       executeFunctionCall5(const std::string& function,const std::string& language,std::vector<float>& parameters);
    std::string       executeFunctionCall5(const std::string& function,const std::string& language,std::vector<double>& parameters);


    std::string       executeFunctionCall6(const std::string& function,std::vector<std::string>& params);

    std::string       executeFunctionCall6(
                        const std::string& function,
                        const std::string& producerName,
                        const std::string& parameterName,
                        T::ParamKeyType parameterKeyType,
                        const std::string& parameterKey,
                        short parameterLevelId,
                        int parameterLevel,
                        short forecastType,
                        short forecastNumber,
                        short interpolationMethod);


    // Type 9: Takes 1 to N vectors and returns one vector
    void              executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    void              executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    void*             getLuaState(ulonglong& key);
    void              releaseLuaState(ulonglong key);

    void              print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void              loadFile();
    void              loadFunctionList(uint type);

    std::string       mFilename;
    LuaFunction_map   mFunctions;
    void*             mLuaState[NUM_OF_LUA_HANDLES];
    ulonglong         mStateKey[NUM_OF_LUA_HANDLES];
    ulonglong         mStateKeyCounter;
    time_t            mLastModified;
    ModificationLock  mModificationLock;
    ModificationLock  mStateModificationLock;
};


class LuaHandle
{
  public:

    LuaHandle(LuaFile *luaFile)
    {
      mLuaFile = luaFile;
      mState = mLuaFile->getLuaState(mKey);
    }

    ~LuaHandle()
    {
      mLuaFile->releaseLuaState(mKey);
    }

    void* getState()
    {
      return mState;
    }

  protected:

    LuaFile    *mLuaFile;
    void  *mState;
    ulonglong  mKey;
};

typedef std::vector<LuaFile> LuaFile_vec;


}  // namespace Lua
}  // namespace SmartMet
