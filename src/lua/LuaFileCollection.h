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
                    LuaFileCollection(const string_vec& filenames);
                    LuaFileCollection(const LuaFileCollection& luaFileCollection);
    virtual         ~LuaFileCollection();

    void            init();
    void            init(const string_vec& filenames);
    bool            checkUpdates(bool force);
    bool            getFunction(const std::string& functionName,const uint functionType,std::string& function);
    uint            getFunction(const std::string& functionName,std::string& function);

    // Type 1: Takes one or several parameters, returns one value
    float           executeFunctionCall1(const std::string& function,std::vector<float>& parameters);
    double          executeFunctionCall1(const std::string& function,std::vector<double>& parameters);

    double          executeFunctionCall2(const std::string& function,const char *str);

    // Type 4: Takes two data vectors and coordinate vector, returns one vector
    void            executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    void            executeFunctionCall4(const std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    // Type 5: Takes one or several parameters, returns one string value
    std::string     executeFunctionCall5(const std::string& function,const std::string& language,std::vector<float>& parameters);
    std::string     executeFunctionCall5(const std::string& function,const std::string& language,std::vector<double>& parameters);

    std::string     executeFunctionCall6(const std::string& function,std::vector<std::string>& params);

    std::string     executeFunctionCall6(
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

    void            executeFunctionCall8(const std::string& function,const char *subFunction,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    void            executeFunctionCall8(const std::string& function,const char *subFunction,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    // Type 9: Takes 1 to N vectors and returns one vector
    void            executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters);
    void            executeFunctionCall9(const std::string& function,uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters);

    void            print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    string_vec        mFilenames;
    LuaFile_vec       mLuaFileList;
    ModificationLock  mModificationLock;
    time_t            mLastCheck;
    uint              mCheckInterval;
};



}  // namespace QueryServer
}  // namespace SmartMet
