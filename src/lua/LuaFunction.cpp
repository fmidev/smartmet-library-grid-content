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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





LuaFunction::LuaFunction(std::string functionName,uint type)
{
  try
  {
    mFunctionName = functionName;
    mType = type;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





LuaFunction::~LuaFunction()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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
    throw Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}  // namespace Lua
}  // namespace SmartMet
