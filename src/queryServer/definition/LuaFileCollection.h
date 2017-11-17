#pragma once

#include "LuaFile.h"


namespace SmartMet
{
namespace QueryServer
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
    void            checkUpdates();
    bool            getFunction(std::string& functionName,std::string& function);
    double          executeFunction(std::string& function,std::vector<double>& parameters);
    void            print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    string_vec      mFilenames;
    LuaFile_vec     mLuaFileList;
    ThreadLock      mThreadLock;
};



}  // namespace QueryServer
}  // namespace SmartMet
