#include "Function_sdevDir.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{




#define PI 3.1415926535898

/*! \brief Circular standard deviation in degrees of the given directions (degrees).
    Missing values are ignored. (The result used to be converted from radians with pi/180
    instead of 180/pi, and missing values were used as directions.)
*/

static double directionStdDev(const std::vector<double>& directions)
{
  double xsin = 0;
  double xcos = 0;
  uint count = 0;
  for (auto val : directions)
  {
    if (val == ParamValueMissing)
      continue;
    xsin += sin(val * (PI/180.0));
    xcos += cos(val * (PI/180.0));
    count++;
  }

  if (count == 0)
    return ParamValueMissing;

  if (count < 2)
    return 0.0;

  xsin /= count;
  xcos /= count;

  // Rounding may make the mean resultant length slightly larger than one
  double r2 = std::min(1.0,xsin*xsin+xcos*xcos);
  double stddev = sqrt(-log(r2));
  return stddev*180/PI;
}


/*! \brief Function: Constructor. */

Function_sdevDir::Function_sdevDir()
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

Function_sdevDir::Function_sdevDir(const Function_sdevDir& function)
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

Function_sdevDir::~Function_sdevDir()
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

float Function_sdevDir::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    std::vector<double> values;
    for (auto val : parameters)
      values.push_back(val);
    return directionStdDev(values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call1. */

double Function_sdevDir::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    std::vector<double> values;
    for (auto val : parameters)
      values.push_back(val);
    return directionStdDev(values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}







/*! \brief Function: Execute function call9. */

void Function_sdevDir::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::vector<double> values;
      for (unsigned int i = 0; i < len; i++)
      {
        if (s < inParameters[i].size())
          values.push_back(inParameters[i][s]);
      }
      outParameters.emplace_back(directionStdDev(values));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





/*! \brief Function: Execute function call9. */

void Function_sdevDir::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    uint sz = columns*rows;
    uint len = inParameters.size();
    outParameters.reserve(sz);

    for (uint s=0; s<sz; s++)
    {
      std::vector<double> values;
      for (unsigned int i = 0; i < len; i++)
      {
        if (s < inParameters[i].size())
          values.push_back(inParameters[i][s]);
      }
      outParameters.emplace_back(directionStdDev(values));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




/*! \brief Function: Duplicate. */

Function* Function_sdevDir::duplicate()
{
  try
  {
    return (Function*)new Function_sdevDir(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






}  // namespace Functions
}  // namespace SmartMet
