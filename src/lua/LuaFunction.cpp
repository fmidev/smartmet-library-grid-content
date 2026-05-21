#include "LuaFunction.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Lua
{


/*! \brief Constructor. */

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





/*! \brief Constructor. */

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





/*! \brief Constructor. */

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





/*! \brief Destructor. */

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





/*! \brief Print. */

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
