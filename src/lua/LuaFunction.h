#pragma once

#include <grid-files/common/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include <map>


namespace SmartMet
{
namespace Lua
{


class LuaFunction
{
  public:
                   LuaFunction();
                   LuaFunction(const LuaFunction& luaFunction);
                   LuaFunction(std::string functionName,uint type);
    virtual        ~LuaFunction();

    void           print(std::ostream& stream,uint level,uint optionFlags);

    std::string    mFunctionName;
    uint           mType;
};


typedef std::map<std::string,LuaFunction> LuaFunction_map;


}  // namespace Lua
}  // namespace SmartMet
