#pragma once

#include <grid-files/common/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include <unordered_map>


namespace SmartMet
{
namespace Lua
{


class LuaFunction
{
  public:
                   LuaFunction();
                   LuaFunction(const LuaFunction& luaFunction);
                   LuaFunction(const std::string& functionName,const uint type);
    virtual        ~LuaFunction();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mFunctionName;
    uint           mType;
};


typedef std::unordered_map<std::string,LuaFunction> LuaFunction_map;


}  // namespace Lua
}  // namespace SmartMet
