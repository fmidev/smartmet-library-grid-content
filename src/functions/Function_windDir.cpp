#include "Function_windDir.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_windDir::Function_windDir()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_windDir::Function_windDir(const Function_windDir& function)
:Function(function)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_windDir::~Function_windDir()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_windDir::executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    double PI = 3.14159265;
    uint len = inParameters1.size();

    outParameters.reserve(len);

    for (uint t=0; t<len; t++)
    {
      double a = angles[t];
      double u = inParameters1[t];
      double v = inParameters2[t];
      double b = atan(v/u);
      double c = 0;
      double d = 0;

      if (u >= 0  && v >= 0)
        c = b;
      else
      if (u < 0  && v >= 0)
        c = b + PI;
      else
      if (u < 0  && v < 0)
        c = b + PI;
      else
      if (u >= 0  and  v < 0)
        c = b;

      if (a < (PI/2))
        d = c - a;
      else
        d = c - (PI-a);

      outParameters.push_back(270-((d*180)/PI));
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_windDir::executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    double PI = 3.14159265;
    uint len = inParameters1.size();

    outParameters.reserve(len);

    for (uint t=0; t<len; t++)
    {
      double a = angles[t];
      double u = inParameters1[t];
      double v = inParameters2[t];
      double b = atan(v/u);
      double c = 0;
      double d = 0;

      if (u >= 0  && v >= 0)
        c = b;
      else
      if (u < 0  && v >= 0)
        c = b + PI;
      else
      if (u < 0  && v < 0)
        c = b + PI;
      else
      if (u >= 0  and  v < 0)
        c = b;

      if (a < (PI/2))
        d = c - a;
      else
        d = c - (PI-a);

      outParameters.push_back(270-((d*180)/PI));
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function* Function_windDir::duplicate()
{
  try
  {
    return (Function*)new Function_windDir(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




void Function_windDir::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_windDir\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
