#pragma once

#include "LuaFunction.h"
#include <grid-files/common/Coordinate.h>
#include <grid-files/grid/Typedefs.h>


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

    // Type 4: Takes two data vectors and angle vector, returns one vector
    void            executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters);
    void            executeFunctionCall4(std::string& function,uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters);

    std::string     executeFunctionCall5(std::string& function,std::string language,std::vector<float>& parameters);
    std::string     executeFunctionCall5(std::string& function,std::string language,std::vector<double>& parameters);


    double          executeFunctionCall6(
                      std::string& function,
                      short interpolationMethod,
                      std::string& producerName,
                      uint producerId,
                      uint generationId,
                      std::string& analysisTime,
                      std::string& forecastTime,
                      std::string& parameterName,
                      T::ParamKeyType parameterKeyType,
                      std::string& parameterKey,
                      T::ParamLevelIdType parameterLevelIdType,
                      short parameterLevelId,
                      int parameterLevel,
                      short forecastType,
                      short forecastNumber,
                      double longitude,
                      double latitude,
                      double dem,
                      short coverType,
                      double x,
                      double y,
                      double val_bl,
                      double val_br,
                      double val_tr,
                      double val_tl);

    std::string     executeFunctionCall7(
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
                      short interpolationMethod);

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
