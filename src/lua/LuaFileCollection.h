#pragma once

#include "LuaFile.h"


namespace SmartMet
{
namespace Lua
{

class LuaFileCollection
{
  public:
                    LuaFileCollection();
                    LuaFileCollection(string_vec& filenames);
                    LuaFileCollection(const LuaFileCollection& luaFileCollection);
    virtual         ~LuaFileCollection();

    void            init();
    void            init(string_vec& filenames);
    void            checkUpdates(bool force);
    bool            getFunction(std::string& functionName,uint functionType,std::string& function);

    // Type 1: Takes one or several parameters, returns one value
    float           executeFunctionCall1(std::string& function,std::vector<float>& parameters);
    double          executeFunctionCall1(std::string& function,std::vector<double>& parameters);

    // Type 2: Takes and returns an vector
    void            executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inOutParameters);
    void            executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<float>& inParameters,std::vector<float>& outParameters);
    void            executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inOutParameters);
    void            executeFunctionCall2(std::string& function,uint columns,uint rows,std::vector<double>& inParameters,std::vector<double>& outParameters);

    // Type 3: Takes two vectors and returns one vector
    void            executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters);
    void            executeFunctionCall3(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters);

    void            print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    string_vec      mFilenames;
    LuaFile_vec     mLuaFileList;
    ThreadLock      mThreadLock;
    time_t          mLastCheck;
    uint            mCheckInterval;
};



}  // namespace QueryServer
}  // namespace SmartMet
