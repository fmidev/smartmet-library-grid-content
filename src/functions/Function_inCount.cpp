#include "Function_inCount.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


/*! \brief Function: Constructor. */

Function_inCount::Function_inCount()
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

Function_inCount::Function_inCount(const Function_inCount& function)
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

Function_inCount::~Function_inCount()
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

float Function_inCount::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint count = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val >= min  &&  val <= max)
        count++;
    }

    return (float)count;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Execute function call1. */

double Function_inCount::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint count = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val >= min  &&  val <= max)
        count++;
    }

    return (double)count;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Execute function call9. */

void Function_inCount::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      uint count = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val >= min  &&  val <= max)
          count++;
      }
      outParameters.emplace_back(count);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_inCount::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (extParameters.size() != 2)
      return;

    double min = extParameters[0];
    double max = extParameters[1];

    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      uint count = 0;
      for (uint t=0; t<len; t++)
      {
        double val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val >= min  &&  val <= max)
          count++;
      }
      outParameters.emplace_back((double)count);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_inCount::duplicate()
{
  try
  {
    return (Function*)new Function_inCount(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
