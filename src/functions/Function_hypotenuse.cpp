#include "Function_hypotenuse.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_hypotenuse::Function_hypotenuse()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_hypotenuse::Function_hypotenuse(const Function_hypotenuse& function)
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




Function_hypotenuse::~Function_hypotenuse()
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





float Function_hypotenuse::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double a = parameters[0];
    double b = parameters[1];

    if (a != ParamValueMissing and b != ParamValueMissing)
      return (float)sqrt(a*a+b*b);
    else
      return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_hypotenuse::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double a = parameters[0];
    double b = parameters[1];

    if (a != ParamValueMissing and b != ParamValueMissing)
      return (float)sqrt(a*a+b*b);
    else
      return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






void Function_hypotenuse::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      float a = ParamValueMissing;
      float b = ParamValueMissing;

      if (t < inParameters[0].size())
        a = inParameters[0][t];

      if (t < inParameters[1].size())
        b = inParameters[1][t];


      if (a != ParamValueMissing and b != ParamValueMissing)
      {
        float c = (float)sqrt(a*a+b*b);
        outParameters.emplace_back(c);
      }
      else
      {
        outParameters.emplace_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_hypotenuse::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = ParamValueMissing;
      double b = ParamValueMissing;

      if (t < inParameters[0].size())
        a = inParameters[0][t];

      if (t < inParameters[1].size())
        b = inParameters[1][t];

      if (a != ParamValueMissing and b != ParamValueMissing)
      {
        double c = (double)sqrt(a*a+b*b);
        outParameters.emplace_back(c);
      }
      else
      {
        outParameters.emplace_back(ParamValueMissing);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_hypotenuse::duplicate()
{
  try
  {
    return (Function*)new Function(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_hypotenuse::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_hypotenuse\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
