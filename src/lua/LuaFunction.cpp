#include "LuaFunction.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Lua
{


LuaFunction::LuaFunction()
{
  try
  {
    mType = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





LuaFunction::LuaFunction(const LuaFunction& luaFunction)
{
  try
  {
    mFunctionName = luaFunction.mFunctionName;
    mType = luaFunction.mType;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





LuaFunction::LuaFunction(const std::string& functionName,const uint type)
{
  try
  {
    mFunctionName = functionName;
    mType = type;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





LuaFunction::~LuaFunction()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void LuaFunction::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LuaFunction\n";
    stream << space(level) << "- mFunctionName  = " << mFunctionName << "\n";
    stream << space(level) << "- mType          = " << mType << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



}  // namespace Lua
}  // namespace SmartMet
