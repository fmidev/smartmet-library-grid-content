#pragma once

#include "LuaFunction.h"


namespace SmartMet
{
namespace QueryServer
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
    bool            getFunction(std::string& functionName,std::string& function);
    double          executeFunction(std::string& function,std::vector<double>& parameters);
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


}  // namespace QueryServer
}  // namespace SmartMet
