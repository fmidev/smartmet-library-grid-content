#pragma once

#include "LuaFunction.h"


namespace SmartMet
{
namespace Lua
{

class LuaFile
{
  public:
                    LuaFile();
                    LuaFile(std::string filename);
                    LuaFile(const LuaFile& luaFile);
    virtual         ~LuaFile();

    void            init();
    void            init(std::string filename);
    void            checkUpdates();
    uint            getFunction(std::string& functionName,std::string& function);

    // Type 1: Takes one or several parameters, returns one value
    float           executeFunctionType1(std::string& function,std::vector<float>& parameters);
    double          executeFunctionType1(std::string& function,std::vector<double>& parameters);

    // Type 2: Takes and returns an vector
    void            executeFunctionType2(std::string& function,std::vector<float>& inOutParameters);
    void            executeFunctionType2(std::string& function,std::vector<float>& inParameters,std::vector<float>& outParameters);
    void            executeFunctionType2(std::string& function,std::vector<double>& inOutParameters);
    void            executeFunctionType2(std::string& function,std::vector<double>& inParameters,std::vector<double>& outParameters);

    // Type 3: Takes two vectors and returns one vector
    void            executeFunctionType3(std::string& function,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters);
    void            executeFunctionType3(std::string& function,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters);

    void            print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    void            loadFile();
    void            loadFunctionList(uint type);

    std::string     mFilename;
    LuaFunction_map mFunctions;
    void*           mLuaState;
    time_t          mLastModified;
    ThreadLock      mThreadLock;
};


typedef std::vector<LuaFile> LuaFile_vec;


}  // namespace Lua
}  // namespace SmartMet