#include "Function_out.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_out::Function_out()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_out::Function_out(const Function_out& function)
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





Function_out::~Function_out()
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





float Function_out::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        if (parameters[0] < parameters[1]  &&  parameters[0] > parameters[2])
          return 1.0;

        return 0.0;
      }
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain three values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_out::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        if (parameters[0] < parameters[1]  &&  parameters[0] > parameters[2])
          return 1.0;

        return 0.0;
      }
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain three values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_out::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      float val2 = extParameters[0];
      float val3 = extParameters[1];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
          {
            if (val1 < val2  ||  val1 > val3)
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_out::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      double val2 = extParameters[0];
      double val3 = extParameters[1];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[0].size())
        {
          double val1 = inParameters[0][s];
          if (val1 != ParamValueMissing &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
          {
            if (val1 < val2  ||  val1 > val3)
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_out::duplicate()
{
  try
  {
    return (Function*)new Function_out(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
