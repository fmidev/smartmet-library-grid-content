#include "Function_inPrcnt.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_inPrcnt::Function_inPrcnt()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_inPrcnt::Function_inPrcnt(const Function_inPrcnt& function)
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





Function_inPrcnt::~Function_inPrcnt()
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





void Function_inPrcnt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
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
        if (val >= min  &&  val <= max)
          agree++;
      }
      if (missing == sz)
        outParameters.push_back(ParamValueMissing);
      else
        outParameters.push_back((float)agree*100/(float)len);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_inPrcnt::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
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
        if (val >= min  &&  val <= max)
          agree++;
      }
      if (missing == sz)
        outParameters.push_back(ParamValueMissing);
      else
        outParameters.push_back((double)agree*100/(double)len);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_inPrcnt::duplicate()
{
  try
  {
    return (Function*)new Function_inPrcnt(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
