#include "Function_streamU.h"
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace Functions
{


Function_streamU::Function_streamU()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_streamU::Function_streamU(const Function_streamU& function)
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




Function_streamU::~Function_streamU()
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



float Function_streamU::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double angle_deg = parameters[0];
    double speed = parameters[1];

    if (angle_deg != ParamValueMissing and speed != ParamValueMissing)
    {
      double rad = angle_deg * M_PI / 180.0;
      double u = speed * sin(rad);

      return (float)u;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_streamU::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 2)
      return ParamValueMissing;

    double angle_deg = parameters[0];
    double speed = parameters[1];

    if (angle_deg != ParamValueMissing and speed != ParamValueMissing)
    {
      double rad = angle_deg * M_PI / 180.0;
      double u = speed * sin(rad);

      return (float)u;
    }

    return ParamValueMissing;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






void Function_streamU::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      float angle_deg = ParamValueMissing;
      float speed = ParamValueMissing;

      if (t < inParameters[0].size())
        angle_deg = inParameters[0][t];

      if (t < inParameters[1].size())
        speed = inParameters[1][t];


      if (angle_deg != ParamValueMissing and speed != ParamValueMissing)
      {
        double rad = angle_deg * M_PI / 180.0;
        double u = speed * sin(rad);

        outParameters.emplace_back(u);
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





void Function_streamU::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() != 2)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double angle_deg = ParamValueMissing;
      double speed = ParamValueMissing;

      if (t < inParameters[0].size())
        angle_deg = inParameters[0][t];

      if (t < inParameters[1].size())
        speed = inParameters[1][t];


      if (angle_deg != ParamValueMissing and speed != ParamValueMissing)
      {
        double rad = angle_deg * M_PI / 180.0;
        double u = speed * sin(rad);

        outParameters.emplace_back(u);
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




Function* Function_streamU::duplicate()
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




void Function_streamU::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_streamU\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
