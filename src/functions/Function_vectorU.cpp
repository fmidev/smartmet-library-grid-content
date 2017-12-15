#include "Function_vectorU.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_vectorU::Function_vectorU()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_vectorU::Function_vectorU(const Function_vectorU& function)
:Function(function)
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_vectorU::~Function_vectorU()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_vectorU::executeFunctionCall4(uint columns,uint rows,std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& angles,std::vector<float>& outParameters)
{
  try
  {
    double PI = 3.14159265;
    uint len = (uint)inParameters1.size();

    outParameters.reserve(len);

    for (uint t=0; t<len; t++)
    {
      double a = angles[t];
      double u = inParameters1[t];
      double v = inParameters2[t];
      double b = atan(v/u);
      double hh = sqrt(u*u + v*v);
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

      double val = hh * cos(d);
      outParameters.push_back(val);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_vectorU::executeFunctionCall4(uint columns,uint rows,std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<float>& angles,std::vector<double>& outParameters)
{
  try
  {
    double PI = 3.14159265;
    uint len = (uint)inParameters1.size();

    outParameters.reserve(len);

    for (uint t=0; t<len; t++)
    {
      double a = angles[t];
      double u = inParameters1[t];
      double v = inParameters2[t];
      double b = atan(v/u);
      double hh = sqrt(u*u + v*v);
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


      double val = hh * cos(d);
      outParameters.push_back(val);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function* Function_vectorU::duplicate()
{
  try
  {
    return (Function*)new Function_vectorU(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void Function_vectorU::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_vectorU\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}


}  // namespace Functions
}  // namespace SmartMet
