#include "Function_eq.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



Function_eq::Function_eq()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_eq::Function_eq(const Function_eq& function)
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





Function_eq::~Function_eq()
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





float Function_eq::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        if (parameters[0] == parameters[1])
          return 1.0;

        return 0.0;
      }
      else
        return ParamValueMissing;
    }

    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[2]);
        if ((long long)(parameters[0]*mp+0.5) == (long long)(parameters[1]*mp+0.5))
          return 1.0;

        return 0.0;
      }
      else
        return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain at least two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_eq::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() == 2)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing)
      {
        if (parameters[0] == parameters[1])
          return 1.0;

        return 0.0;
      }
      else
        return ParamValueMissing;
    }

    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[2]);
        if ((long long)(parameters[0]*mp+0.5) == (long long)(parameters[1]*mp+0.5))
          return 1.0;

        return 0.0;
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







void Function_eq::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 1)
    {
      float val2 = extParameters[0];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (val2 != ParamValueMissing  &&  s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            if (val1 == val2)
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      float val2 = extParameters[0];
      double mp = pow(10,extParameters[1]);

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (val2 != ParamValueMissing  &&  s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            if ((long long)(mp*val1+0.5) == (long long)(mp*val2+0.5))
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 2)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing &&  val2 != ParamValueMissing)
        {
          if (val1 == val2)
            val = 1.0;
          else
            val = 0.0;
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      double mp = pow(10,extParameters[0]);
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing &&  val2 != ParamValueMissing)
        {
          if ((long long)(mp*val1+0.5) == (long long)(mp*val2+0.5))
            val = 1.0;
          else
            val = 0.0;
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





void Function_eq::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    outParameters.reserve(sz);

    if (inParameters.size() == 1  &&  extParameters.size() == 1)
    {
      float val2 = extParameters[0];

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (val2 != ParamValueMissing  &&  s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            if (val1 == val2)
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      float val2 = extParameters[0];
      double mp = pow(10,extParameters[1]);

      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        if (val2 != ParamValueMissing  &&  s < inParameters[0].size())
        {
          float val1 = inParameters[0][s];
          if (val1 != ParamValueMissing)
          {
            if ((long long)(mp*val1+0.5) == (long long)(mp*val2+0.5))
              val = 1.0;
            else
              val = 0.0;
          }
        }
        outParameters.emplace_back(val);
      }
    }

    if (inParameters.size() == 2)
    {
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing &&  val2 != ParamValueMissing)
        {
          if (val1 == val2)
            val = 1.0;
          else
            val = 0.0;
        }
        outParameters.emplace_back(val);
      }
    }


    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      double mp = pow(10,extParameters[0]);
      for (uint s=0; s<sz; s++)
      {
        float val = ParamValueMissing;
        float val1 = ParamValueMissing;
        float val2 = ParamValueMissing;

        if (s < inParameters[0].size())
          val1 = inParameters[0][s];

        if (s < inParameters[1].size())
          val2 = inParameters[1][s];

        if (val1 != ParamValueMissing &&  val2 != ParamValueMissing)
        {
          if ((long long)(mp*val1+0.5) == (long long)(mp*val2+0.5))
            val = 1.0;
          else
            val = 0.0;
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




Function* Function_eq::duplicate()
{
  try
  {
    return (Function*)new Function_eq(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
