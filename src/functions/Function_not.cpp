#include "Function_not.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_not::Function_not()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_not::Function_not(const Function_not& function)
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





Function_not::~Function_not()
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





float Function_not::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
      {
        if (parameters[0] == 0)
          return 1.0;
        else
          return 0;
      }
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





double Function_not::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 1)
    {
      if (parameters[0] != ParamValueMissing)
      {
        if (parameters[0] == 0)
          return 1.0;
        else
          return 0;
      }
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







void Function_not::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
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
        float val1 = ParamValueMissing;
        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (val1 != ParamValueMissing)
        {
          if (val1 == 0)
            val = 1;
          else
            val = 0;
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





void Function_not::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
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
        float val1 = ParamValueMissing;
        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (val1 != ParamValueMissing)
        {
          if (val1 == 0)
            val = 1;
          else
            val = 0;
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




Function* Function_not::duplicate()
{
  try
  {
    return (Function*)new Function_not(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
