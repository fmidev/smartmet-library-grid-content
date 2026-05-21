#include "Function_windDir.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


/*! \brief Function: Constructor. */

Function_windDir::Function_windDir()
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

Function_windDir::Function_windDir(const Function_windDir& function)
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

Function_windDir::~Function_windDir()
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

float Function_windDir::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double u = parameters[0];
    double v = parameters[1];

    if (u != ParamValueMissing and v != ParamValueMissing)
    {
      double angle_rad = atan2(-u, -v);
      double angle_deg = angle_rad * 180.0 / M_PI;

      if (angle_deg < 0)
        angle_deg += 360.0;

      return (float)angle_deg;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_windDir::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double u = parameters[0];
    double v = parameters[1];

    if (u != ParamValueMissing and v != ParamValueMissing)
    {
      double angle_rad = atan2(-u, -v);
      double angle_deg = angle_rad * 180.0 / M_PI;

      if (angle_deg < 0)
        angle_deg += 360.0;

      return (double)angle_deg;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






/*! \brief Function: Execute function call9. */

void Function_windDir::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      float u = ParamValueMissing;
      float v = ParamValueMissing;

      if (t < inParameters[0].size())
        u = inParameters[0][t];

      if (t < inParameters[1].size())
        v = inParameters[1][t];


      if (u != ParamValueMissing and v != ParamValueMissing)
      {
        double angle_rad = atan2(-u, -v);
        double angle_deg = angle_rad * 180.0 / M_PI;

        if (angle_deg < 0)
          angle_deg += 360.0;

        outParameters.emplace_back(angle_deg);
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





/*! \brief Function: Execute function call9. */

void Function_windDir::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double u = ParamValueMissing;
      double v = ParamValueMissing;

      if (t < inParameters[0].size())
        u = inParameters[0][t];

      if (t < inParameters[1].size())
        v = inParameters[1][t];

      if (u != ParamValueMissing and v != ParamValueMissing)
      {
        double angle_rad = atan2(-u, -v);
        double angle_deg = angle_rad * 180.0 / M_PI;

        if (angle_deg < 0)
          angle_deg += 360.0;

        outParameters.emplace_back(angle_deg);
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




/*! \brief Function: Duplicate. */

Function* Function_windDir::duplicate()
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




/*! \brief Function: Print. */

void Function_windDir::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_windDir\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
