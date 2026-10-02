#include "Function_or.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



/*! \brief Function: Constructor. */

Function_or::Function_or()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Constructor. */

Function_or::Function_or(const Function_or& function)
:Function(function)
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Destructor. */

Function_or::~Function_or()
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





/*! \brief Function: Execute function call1. */

float Function_or::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    // Any true value decides, otherwise a missing value makes the result missing.
    // (The result used to depend on the order of the parameters.)
    uint sz = parameters.size();
    if (sz < 2)
      return ParamValueMissing;

    bool missing = false;
    for (uint t=0; t<sz; t++)
    {
      if (parameters[t] == ParamValueMissing)
        missing = true;
      else if (parameters[t] != 0)
        return 1.0;
    }

    if (missing)
      return ParamValueMissing;
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_or::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    // Any true value decides, otherwise a missing value makes the result missing.
    // (The result used to depend on the order of the parameters.)
    uint sz = parameters.size();
    if (sz < 2)
      return ParamValueMissing;

    bool missing = false;
    for (uint t=0; t<sz; t++)
    {
      if (parameters[t] == ParamValueMissing)
        missing = true;
      else if (parameters[t] != 0)
        return 1.0;
    }

    if (missing)
      return ParamValueMissing;
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_or::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);
    uint len = inParameters.size();
    if (len >= 2)
    {
      for (uint s=0; s<sz; s++)
      {
        // Same rules as in executeFunctionCall1. (A missing value used to give 1 in OR.)
        bool missing = false;
        bool decided = false;
        for (uint t=0; t<len && !decided; t++)
        {
          if (s >= inParameters[t].size() || inParameters[t][s] == ParamValueMissing)
            missing = true;
          else if (inParameters[t][s] != 0)
            decided = true;
        }
        if (decided)
          outParameters.emplace_back(1);
        else if (missing)
          outParameters.emplace_back(ParamValueMissing);
        else
          outParameters.emplace_back(0);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_or::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);
    uint len = inParameters.size();
    if (len >= 2)
    {
      for (uint s=0; s<sz; s++)
      {
        // Same rules as in executeFunctionCall1. (A missing value used to give 1 in OR.)
        bool missing = false;
        bool decided = false;
        for (uint t=0; t<len && !decided; t++)
        {
          if (s >= inParameters[t].size() || inParameters[t][s] == ParamValueMissing)
            missing = true;
          else if (inParameters[t][s] != 0)
            decided = true;
        }
        if (decided)
          outParameters.emplace_back(1);
        else if (missing)
          outParameters.emplace_back(ParamValueMissing);
        else
          outParameters.emplace_back(0);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_or::duplicate()
{
  try
  {
    return (Function*)new Function_or(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
