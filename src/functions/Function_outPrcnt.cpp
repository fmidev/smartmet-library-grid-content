#include "Function_outPrcnt.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_outPrcnt::Function_outPrcnt()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_outPrcnt::Function_outPrcnt(const Function_outPrcnt& function)
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





Function_outPrcnt::~Function_outPrcnt()
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





float Function_outPrcnt::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint agree = 0;
    uint missing = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val == ParamValueMissing)
        missing++;
      else
      if (val < min  ||  val > max)
        agree++;
    }

    if (missing == (len-2))
      return ParamValueMissing;

    return (float)agree*100/(float)(len-2);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




double Function_outPrcnt::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() < 3)
      return ParamValueMissing;

    double min = parameters[0];
    double max = parameters[1];

    uint agree = 0;
    uint missing = 0;

    uint len = parameters.size();
    for (uint s=2; s<len; s++)
    {
      double val = parameters[s];
      if (val == ParamValueMissing)
        missing++;
      else
      if (val < min  ||  val > max)
        agree++;
    }

    if (missing == (len-2))
      return ParamValueMissing;

    return (double)agree*100/(double)(len-2);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_outPrcnt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
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
      uint agree = 0;
      uint missing = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val == ParamValueMissing)
          missing++;
        else
        if (val < min  || val > max)
          agree++;
      }
      if (missing == sz)
        outParameters.emplace_back(ParamValueMissing);
      else
        outParameters.emplace_back((float)agree*100/(float)len);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_outPrcnt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
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
      uint agree = 0;
      uint missing = 0;
      for (uint t=0; t<len; t++)
      {
        double val = ParamValueMissing;
        if (s < inParameters[t].size())
          val = inParameters[t][s];

        if (val == ParamValueMissing)
          missing++;
        else
        if (val < min  ||  val > max)
          agree++;
      }
      if (missing == sz)
        outParameters.emplace_back(ParamValueMissing);
      else
        outParameters.emplace_back((double)agree*100/(double)len);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_outPrcnt::duplicate()
{
  try
  {
    return (Function*)new Function_outPrcnt(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
