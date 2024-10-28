#include "Function_div.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_div::Function_div()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_div::Function_div(const Function_div& function)
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





Function_div::~Function_div()
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





float Function_div::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    float value = parameters[0];
    if (value == ParamValueMissing)
      return ParamValueMissing;

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val == ParamValueMissing)
        return ParamValueMissing;

      if (val != 0)
        value = value / val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_div::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    uint len = parameters.size();
    if (len == 0)
      return 0;

    double value = parameters[0];
    if (value == ParamValueMissing)
      return ParamValueMissing;

    for (uint t=1; t<len; t++)
    {
      auto val = parameters[t];
      if (val == ParamValueMissing)
        return ParamValueMissing;

      if (val != 0)
        value = value / val;
    }
    return value;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    uint elen = extParameters.size();
    outParameters.reserve(sz);

    float b = 1;
    if (elen > 0)
    {
      for (uint t=0; t<elen; t++)
      {
        if (extParameters[t] == ParamValueMissing)
        {
          b = ParamValueMissing;
          t = elen;
        }
        else
        {
          b = b * extParameters[t];
        }
      }
    }

    for (uint s=0; s<sz; s++)
    {
      float value = ParamValueMissing;
      if (s < inParameters[0].size())
      {
        value = inParameters[0][s];
        if (value != ParamValueMissing)
        {
          for (uint t=1; t<len; t++)
          {
            if (s < inParameters[t].size())
            {
              double val = inParameters[t][s];
              if (val == ParamValueMissing)
              {
                value = ParamValueMissing;
                t = len;
              }
              else
              {
                // If the divisor is zero then we ignore the division. Should we return ParamValueMissing instead?
                if (val != 0)
                  value = value / val;
              }
            }
            else
            {
              value = ParamValueMissing;
              t = len;
            }
          }
        }
        if (b == ParamValueMissing)
          value = ParamValueMissing;

        if (b != 1  &&  b != 0  &&  value != ParamValueMissing)
          value = value / b;
      }

      outParameters.emplace_back(value);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_div::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    uint elen = extParameters.size();
    outParameters.reserve(sz);

    double b = 1;
    if (elen > 0)
    {
      for (uint t=0; t<elen; t++)
      {
        if (extParameters[t] == ParamValueMissing)
        {
          b = ParamValueMissing;
          t = elen;
        }
        else
        {
          b = b * extParameters[t];
        }
      }
    }

    for (uint s=0; s<sz; s++)
    {
      double value = ParamValueMissing;
      if (s < inParameters[0].size())
      {
        value = inParameters[0][s];
        if (value != ParamValueMissing)
        {
          for (uint t=1; t<len; t++)
          {
            if (s < inParameters[t].size())
            {
              double val = inParameters[t][s];
              if (val == ParamValueMissing)
              {
                value = ParamValueMissing;
                t = len;
              }
              else
              {
                // If the divisor is zero then we ignore the division. Should we return ParamValueMissing instead?
                if (val != 0)
                  value = value / val;
              }
            }
            else
            {
              value = ParamValueMissing;
              t = len;
            }
          }
        }
        if (b == ParamValueMissing)
          value = ParamValueMissing;

        if (b != 1  &&  b != 0  &&  value != ParamValueMissing)
          value = value / b;
      }
      outParameters.emplace_back(value);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_div::duplicate()
{
  try
  {
    return (Function*)new Function_div(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
