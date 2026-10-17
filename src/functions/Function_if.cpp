#include "Function_if.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{



namespace
{

/*! \brief Return the grid value at the given index (as a float), or missing if the grid is too short. */

template <typename T>
float gridValue(const std::vector<T>& grid,uint s)
{
  if (s < grid.size())
    return grid[s];

  return ParamValueMissing;
}



/*! \brief Return the "else" value val3 if the condition val1 is zero and the "then" value val2 otherwise, or missing if any of the values is missing. */

float selectValue(float val1,float val2,float val3)
{
  if (val1 == ParamValueMissing  ||  val2 == ParamValueMissing  ||  val3 == ParamValueMissing)
    return ParamValueMissing;

  if (val1 == 0)
    return val3;

  return val2;
}



/*! \brief Select the grid values by the condition grid: the "then" and "else" values are grids or scalars. */

template <typename T>
void executeIf(uint columns,uint rows,const std::vector<std::vector<T>>& inParameters,const std::vector<double>& extParameters,std::vector<T>& outParameters)
{
  uint sz = columns*rows;
  outParameters.reserve(sz);

  if (inParameters.size() == 3)
  {
    for (uint s=0; s<sz; s++)
      outParameters.emplace_back(selectValue(gridValue(inParameters[0],s),gridValue(inParameters[1],s),gridValue(inParameters[2],s)));
  }

  if (inParameters.size() == 2  &&  extParameters.size() == 1)
  {
    // Notice that in this case we do not know if the scalar parameter is given as "then" or "else" value.
    // I.e. these functions delivers the same input parameters : IF{vector1;vector2;scalar} and IF{vector1;scalar;vector2}.
    // That's why we need to assume that if the condition is true then we always return value from the vector inParameter[1].
    // Otherwise we return the scalar value.

    float val3 = extParameters[0];

    for (uint s=0; s<sz; s++)
      outParameters.emplace_back(selectValue(gridValue(inParameters[0],s),gridValue(inParameters[1],s),val3));
  }

  if (inParameters.size() == 1  &&  extParameters.size() == 2)
  {
    float val2 = extParameters[0];
    float val3 = extParameters[1];

    for (uint s=0; s<sz; s++)
      outParameters.emplace_back(selectValue(gridValue(inParameters[0],s),val2,val3));
  }
}

}  // namespace



/*! \brief Function: Constructor. */

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





/*! \brief Function: Constructor. */

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





/*! \brief Function: Destructor. */

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





/*! \brief Function: Execute function call1. */

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
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain three values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

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
      return ParamValueMissing;
    }

    throw Fmi::Exception(BCP, "The parameters vector should contain three values!");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_if::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    executeIf(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_if::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    executeIf(columns,rows,inParameters,extParameters,outParameters);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

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
