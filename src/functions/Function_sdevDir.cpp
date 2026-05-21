#include "Function_sdevDir.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{

#define PI 3.1415926535898


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
    double xsin = 0;
    double xcos = 0;
    uint len = parameters.size();
    for (uint i = 0; i < len; i++)
    {
      double val = parameters[i];

      xsin += sin(val * (PI/180.0));
      xcos += cos(val * (PI/180.0));
    }
    xsin /= len;
    xcos /= len;

    double stddev = sqrt(-log(xsin*xsin+xcos*xcos));
    return PI*stddev/180;
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
    double xsin = 0;
    double xcos = 0;
    uint len = parameters.size();
    for (uint i = 0; i < len; i++)
    {
      double val = parameters[i];

      xsin += sin(val * (PI/180.0));
      xcos += cos(val * (PI/180.0));
    }
    xsin /= len;
    xcos /= len;

    double stddev = sqrt(-log(xsin*xsin+xcos*xcos));
    return PI*stddev/180;
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
      double xsin = 0;
      double xcos = 0;
      for (unsigned int i = 0; i < len; i++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[i].size())
           val = inParameters[i][s];

        xsin += sin(val * (PI/180.0));
        xcos += cos(val * (PI/180.0));
      }
      xsin /= len;
      xcos /= len;

      double stddev = sqrt(-log(xsin*xsin+xcos*xcos));
      if (len < 2)
        outParameters.emplace_back(0.0);
      else
        outParameters.emplace_back(PI*stddev/180);
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
      double xsin = 0;
      double xcos = 0;
      for (unsigned int i = 0; i < len; i++)
      {
        float val = ParamValueMissing;
        if (s < inParameters[i].size())
           val = inParameters[i][s];

        xsin += sin(val * (PI/180.0));
        xcos += cos(val * (PI/180.0));
      }
      xsin /= len;
      xcos /= len;

      double stddev = sqrt(-log(xsin*xsin+xcos*xcos));
      if (len < 2)
        outParameters.emplace_back(0.0);
      else
        outParameters.emplace_back(PI*stddev/180);
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
