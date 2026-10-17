#include "Function_eq.h"
#include <grid-files/common/GeneralFunctions.h>
#include <cmath>


namespace SmartMet
{
namespace Functions
{



namespace
{

/*! \brief Return a comparison which tests if the values are equal when rounded with the given multiplier. */

auto roundedEqual(double mp)
{
  return [mp](float val1,float val2) { return (Int64)std::llround(mp*val1) == (Int64)std::llround(mp*val2); };
}

}  // namespace



/*! \brief Function: Constructor. */

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





/*! \brief Function: Constructor. */

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





/*! \brief Function: Destructor. */

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





/*! \brief Function: Execute function call1. */

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
      return ParamValueMissing;
    }

    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[2]);
        if ((Int64)std::llround(parameters[0]*mp) == (Int64)std::llround(parameters[1]*mp))
          return 1.0;

        return 0.0;
      }
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain at least two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

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
      return ParamValueMissing;
    }

    if (parameters.size() == 3)
    {
      if (parameters[0] != ParamValueMissing  &&  parameters[1] != ParamValueMissing  &&  parameters[2] != ParamValueMissing)
      {
        double mp = pow(10,(int)parameters[2]);
        if ((Int64)std::llround(parameters[0]*mp) == (Int64)std::llround(parameters[1]*mp))
          return 1.0;

        return 0.0;
      }
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain two values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_eq::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    compareGridValues(columns,rows,inParameters,extParameters,[](float val1,float val2) { return val1 == val2; },outParameters);

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      // Comparison with the given number of decimals
      double mp = pow(10,extParameters[1]);
      std::vector<double> value = {extParameters[0]};
      compareGridValues(columns,rows,inParameters,value,roundedEqual(mp),outParameters);
    }

    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      // Comparison with the given number of decimals
      double mp = pow(10,extParameters[0]);
      compareGridValues(columns,rows,inParameters,extParameters,roundedEqual(mp),outParameters);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_eq::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    compareGridValues(columns,rows,inParameters,extParameters,[](float val1,float val2) { return val1 == val2; },outParameters);

    if (inParameters.size() == 1  &&  extParameters.size() == 2)
    {
      // Comparison with the given number of decimals
      double mp = pow(10,extParameters[1]);
      std::vector<double> value = {extParameters[0]};
      compareGridValues(columns,rows,inParameters,value,roundedEqual(mp),outParameters);
    }

    if (inParameters.size() == 2  &&  extParameters.size() == 1)
    {
      // Comparison with the given number of decimals
      double mp = pow(10,extParameters[0]);
      compareGridValues(columns,rows,inParameters,extParameters,roundedEqual(mp),outParameters);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

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
