#include "Function_sdev.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_sdev::Function_sdev()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_sdev::Function_sdev(const Function_sdev& function)
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





Function_sdev::~Function_sdev()
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





float Function_sdev::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    double sum = 0;
    double squaredSum = 0;
    uint len = parameters.size();
    uint counter = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        sum = sum + val;
        squaredSum = squaredSum + val*val;
        counter++;
      }
    }
    if (counter > 1)
      return (squaredSum - sum*sum/counter) / (counter-1);

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_sdev::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    double sum = 0;
    double squaredSum = 0;
    uint len = parameters.size();
    uint counter = 0;
    for (uint t=0; t<len; t++)
    {
      auto val = parameters[t];
      if (val != ParamValueMissing)
      {
        sum = sum + val;
        squaredSum = squaredSum + val*val;
        counter++;
      }
    }
    if (counter > 1)
      return (squaredSum - (sum*sum)/counter) / (counter-1);

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_sdev::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double sum = 0;
      double squaredSum = 0;
      uint counter = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          sum = sum + val;
          squaredSum = squaredSum + val*val;
          counter++;
        }
      }
      if (counter > 1)
        outParameters.emplace_back((squaredSum - (sum*sum)/counter) / (counter-1));
      else
        outParameters.emplace_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_sdev::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      double sum = 0;
      double squaredSum = 0;
      uint counter = 0;
      for (uint t=0; t<len; t++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[t].size())
           val = inParameters[t][s];

        if (val != ParamValueMissing)
        {
          sum = sum + val;
          squaredSum = squaredSum + val*val;
          counter++;
        }
      }
      if (counter > 1)
        outParameters.emplace_back((squaredSum - (sum*sum)/counter) / (counter-1));
      else
        outParameters.emplace_back(ParamValueMissing);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_sdev::duplicate()
{
  try
  {
    return (Function*)new Function_sdev(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
