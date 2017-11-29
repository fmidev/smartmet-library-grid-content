#include "Function_hypotenuse.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_hypotenuse::Function_hypotenuse()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





Function_hypotenuse::Function_hypotenuse(const Function_hypotenuse& function)
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




Function_hypotenuse::~Function_hypotenuse()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





float Function_hypotenuse::executeFunctionType1(std::vector<float>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





double Function_hypotenuse::executeFunctionType1(std::vector<double>& parameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function_hypotenuse::executeFunctionType2(std::vector<double>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function_hypotenuse::executeFunctionType2(std::vector<double>& inParameters,std::vector<double>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function_hypotenuse::executeFunctionType2(std::vector<float>& inOutParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function_hypotenuse::executeFunctionType2(std::vector<float>& inParameters,std::vector<float>& outParameters)
{
  throw Spine::Exception(BCP, "Not implemented!");
}





void Function_hypotenuse::executeFunctionType3(std::vector<float>& inParameters1,std::vector<float>& inParameters2,std::vector<float>& outParameters)
{
  try
  {
    uint len = inParameters1.size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double a = inParameters1[t];
      double b = inParameters2[t];

      float c = (float)sqrt(a*a+b*b);
      outParameters.push_back(c);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void Function_hypotenuse::executeFunctionType3(std::vector<double>& inParameters1,std::vector<double>& inParameters2,std::vector<double>& outParameters)
{
  uint len = inParameters1.size();
  outParameters.reserve(len);
  for (uint t=0; t<len; t++)
  {
    double a = inParameters1[t];
    double b = inParameters2[t];

    double c = sqrt(a*a+b*b);
    outParameters.push_back(c);
  }
}




Function* Function_hypotenuse::duplicate()
{
  try
  {
    return (Function*)new Function(*this);
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}




void Function_hypotenuse::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_hypotenuse\n";
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}


}  // namespace Functions
}  // namespace SmartMet