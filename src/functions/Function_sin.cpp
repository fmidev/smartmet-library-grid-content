#include "Function_sin.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_sin::Function_sin()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_sin::Function_sin(const Function_sin& function)
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





Function_sin::~Function_sin()
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





float Function_sin::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
        return sin(parameters[0]*degreesToRadiansMultiplier);
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_sin::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
        return sin(parameters[0]*degreesToRadiansMultiplier);
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain only one value!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_sin::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() == 1)
    {
      uint sz = columns*rows;
      outParameters.reserve(sz);

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[0].size())
          val = inParameters[0][s];

        if (val != ParamValueMissing)
          val = sin(val*degreesToRadiansMultiplier);

        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_sin::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() == 1)
    {
      uint sz = columns*rows;
      outParameters.reserve(sz);

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[0].size())
          val = inParameters[0][s];

        if (val != ParamValueMissing)
          val = sin(val*degreesToRadiansMultiplier);

        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_sin::duplicate()
{
  try
  {
    return (Function*)new Function_sin(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
