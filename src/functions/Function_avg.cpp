#include "Function_avg.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


/*! \brief Function: Constructor. */

Function_avg::Function_avg()
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

Function_avg::Function_avg(const Function_avg& function)
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

Function_avg::~Function_avg()
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

float Function_avg::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    float total = 0;
    uint len = parameters.size();
    uint cnt = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        total = total + val;
        cnt++;
      }
    }
    if (cnt > 0)
      return total/cnt;

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_avg::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double total = 0;
    uint len = parameters.size();
    uint cnt = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        total = total + val;
        cnt++;
      }
    }
    if (cnt > 0)
      return total/cnt;

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_avg::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      float total = 0;
      uint cnt = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          total = total + val;
          cnt++;
        }
      }
      if (cnt > 0)
        outParameters.emplace_back(total/cnt);
      else
        outParameters.emplace_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_avg::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double total = 0;
      uint cnt = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          total = total + val;
          cnt++;
        }
      }
      if (cnt > 0)
        outParameters.emplace_back(total/cnt);
      else
        outParameters.emplace_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_avg::duplicate()
{
  try
  {
    return (Function*)new Function_avg(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
