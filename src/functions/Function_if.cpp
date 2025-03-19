#include "Function_if.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_if::Function_if()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_if::Function_if(const Function_if& function)
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





Function_if::~Function_if()
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





float Function_if::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        if (parameters[0] == 0)
          return parameters[2];

        return parameters[1];
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





double Function_if::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        if (parameters[0] == 0)
          return parameters[2];

        return parameters[1];
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







void Function_if::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 3)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;
        float val3 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (s < inParameters[2].size())
          val3 = inParameters[2][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      // Notice that in this case we do not know if the scalar parameter is given as "then" or "else" value.
      // I.e. these functions delivers the same input parameters : IF{vector1;vector2;scalar} and IF{vector1;scalar;vector2}.
      // That's why we need to assume that if the condition is true then we always return value from the vector inParameter[1].
      // Otherwise we return the scalar value.

      float val3 = extParameters[0];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      float val2 = extParameters[0];
      float val3 = extParameters[1];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
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





void Function_if::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 3)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;
        float val3 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (s < inParameters[2].size())
          val3 = inParameters[2][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      float val3 = extParameters[0];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      float val2 = extParameters[0];
      float val3 = extParameters[1];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (val1 != ParamValueMissing  &&  val2 != ParamValueMissing  &&  val3 != ParamValueMissing)
        {
          if (val1 == 0)
            val = val3;
          else
            val = val2;
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




Function* Function_if::duplicate()
{
  try
  {
    return (Function*)new Function_if(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
