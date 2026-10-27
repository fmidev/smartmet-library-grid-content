#pragma once

#include <grid-files/common/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include <unordered_map>


namespace SmartMet
{
namespace Lua
{

// ====================================================================================
/*! \brief Represents a single named function defined inside a Lua script file.
 *
 *  Stores the function name and its call-type category, which determines which
 *  executeFunctionCallN overload is used when invoking it. */
// ====================================================================================

class LuaFunction
{
  public:
                   LuaFunction();
                   LuaFunction(const LuaFunction& luaFunction);
                   LuaFunction(const std::string& functionName,uint type);
    virtual        ~LuaFunction();
    LuaFunction& operator=(const LuaFunction&) = default;

    void           print(std::ostream& stream,uint level,uint optionFlags) const;

    std::string    mFunctionName;  //!< Name of the Lua function as it appears in the script.
    uint           mType;          //!< Call-type category (1, 4, 5, …) that determines the calling convention.
};


using LuaFunction_map = std::unordered_map<std::string,LuaFunction>;  //!< Map from function name to LuaFunction descriptor.


}  // namespace Lua
}  // namespace SmartMet
