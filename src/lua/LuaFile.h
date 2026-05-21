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

// ====================================================================================
/*! \brief Loads and evaluates a single LuaJIT script file and manages concurrent execution.
 *
 *  Maintains a pool of NUM_OF_LUA_HANDLES independent Lua states so that multiple
 *  threads can invoke functions concurrently without serialisation. Supports
 *  hot-reload: checkUpdates() re-reads the file when the mtime changes. */
// ====================================================================================

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

    double            executeFunctionCall2(const std::string& function,const char *str);

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

    void              executeFunctionCall8(const std::string& function,const char *subFunction,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    void              executeFunctionCall8(const std::string& function,const char *subFunction,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    void              executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    void              executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    void*             getLuaState(UInt64 & key);
    void              releaseLuaState(UInt64 key);

    void              print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void              loadFile();
    void              loadFunctionList(uint type);

    std::string       mFilename;             //!< Path to the Lua script file on disk.
    LuaFunction_map   mFunctions;            //!< Map of function names to their LuaFunction descriptors.
    void*             mLuaState[NUM_OF_LUA_HANDLES];   //!< Pool of independent Lua states for concurrent execution.
    UInt64            mStateKey[NUM_OF_LUA_HANDLES];   //!< Checkout keys associated with each Lua state slot.
    UInt64            mStateKeyCounter;      //!< Monotonically increasing counter used to generate unique state keys.
    time_t            mLastModified;         //!< File modification time at the last load, used for change detection.
    ModificationLock  mModificationLock;     //!< Lock protecting the function map and file-reload operations.
    ModificationLock  mStateModificationLock; //!< Lock protecting checkout/release of Lua state slots.
};


// ====================================================================================
/*! \brief RAII wrapper that checks out a Lua state from a LuaFile and releases it on destruction.
 *
 *  Acquire an instance on the stack before calling any executeFunctionCallN method to
 *  obtain a thread-local Lua state handle. */
// ====================================================================================

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

    LuaFile    *mLuaFile;  //!< Owning LuaFile whose state pool is used.
    void  *mState;         //!< Raw lua_State pointer for the checked-out slot.
    UInt64   mKey;         //!< Unique key used to return this slot to the pool.
};

typedef std::vector<LuaFile> LuaFile_vec;  //!< Ordered collection of LuaFile objects.


}  // namespace Lua
}  // namespace SmartMet
