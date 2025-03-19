#include "Function_round.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_round::Function_round()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_round::Function_round(const Function_round& function)
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





Function_round::~Function_round()
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





float Function_round::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[1]);
        long long nw = (long long)(parameters[0]*mp+0.5);
        return (float)nw/mp;
      }
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_round::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[1]);
        long long nw = (long long)(parameters[0]*mp+0.5);
        return (double)nw/mp;
      }
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







void Function_round::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 1)
    {
      float mp = (long long)pow(10,extParameters[0]);

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (mp != ParamValueMissing  &&  s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            long long nw = (long long)(val1*mp+0.5);
            val = (float)nw/mp;
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





void Function_round::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 1)
    {
      float mp = (long long)pow(10,extParameters[0]);

      for (uint s=0; s<sz; s++)
      {
        double val = ParamValueMissing;
        if (mp != ParamValueMissing  &&  s < inParameters[0].size())
        {
          double val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            long long nw = (long long)(val1*mp+0.5);
            val = (double)nw/mp;
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




Function* Function_round::duplicate()
{
  try
  {
    return (Function*)new Function_round(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
