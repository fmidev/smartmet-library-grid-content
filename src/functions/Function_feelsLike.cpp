#include "Function_feelsLike.h"
#include <grid-files/common/GeneralFunctions.h>
#include <math.h>


namespace SmartMet
{
namespace Functions
{


Function_feelsLike::Function_feelsLike()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





Function_feelsLike::Function_feelsLike(const Function_feelsLike& function)
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




Function_feelsLike::~Function_feelsLike()
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





double Function_feelsLike:: SSI(double rh,double t)
{
  double simmer_limit = 14.5;

  if (t <= simmer_limit)
    return t;

  if (rh == ParamValueMissing)
    return ParamValueMissing;

  if (t == ParamValueMissing)
    return ParamValueMissing;

  double rh_ref = 50.0 / 100.0;
  double r = rh / 100.0;

  double value =  (1.8 * t - 0.55 * (1 - r) * (1.8 * t - 26) - 0.55 * (1 - rh_ref) * 26) / (1.8 * (1 - 0.55 * (1 - rh_ref)));
  return value;
}





double Function_feelsLike::feelsLike(double wind,double rh,double temp,double rad)
{
  try
  {
    if (wind == ParamValueMissing)
      return ParamValueMissing;

    if (rh == ParamValueMissing)
      return ParamValueMissing;

    if (temp == ParamValueMissing)
      return ParamValueMissing;


    // Calculate adjusted wind chill portion. Note that even though
    // the Canadien formula uses km/h, we use m/s and have fitted
    // the coefficients accordingly. Note that (a*w)^0.16 = c*w^16,
    // i.e. just get another coefficient c for the wind reduced to 1.5 meters.

    double a = 15.0;   // using this the two wind chills are good match at T=0
    double t0 = 37.0;  // wind chill is horizontal at this T

    double chill = a + (1 - a / t0) * temp + a / t0 * pow(wind + 1, 0.16) * (temp - t0);

    // Heat index

    double heat = SSI(rh, temp);

    // Add the two corrections together

    double feels = temp + (chill - temp) + (heat - temp);

    // Radiation correction done only when radiation is available
    // Based on the Steadman formula for Apparent temperature,
    // we just inore the water vapour pressure adjustment

    if (rad != ParamValueMissing)
    {

      // Chosen so that at wind=0 and rad=800 the effect is 4 degrees
      // At rad=50 the effect is then zero degrees

      double absorption = 0.07;
      feels = feels + 0.7 * absorption * rad / (wind + 10) - 0.25;
    }

    return feels;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





float Function_feelsLike::executeFunctionCall1(std::vector<float>& parameters)
{
  try
  {
    if (parameters.size() != 4)
      return ParamValueMissing;

    auto wind = parameters[0];
    if (wind == ParamValueMissing)
      return ParamValueMissing;

    auto rh = parameters[1];
    if (rh == ParamValueMissing)
      return ParamValueMissing;

    auto temp = parameters[2];
    if (temp == ParamValueMissing)
      return ParamValueMissing;

    auto rad = parameters[3];

    return feelsLike(wind,rh,temp,rad);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





double Function_feelsLike::executeFunctionCall1(std::vector<double>& parameters)
{
  try
  {
    if (parameters.size() != 4)
      return ParamValueMissing;

    auto wind = parameters[0];
    if (wind == ParamValueMissing)
      return ParamValueMissing;

    auto rh = parameters[1];
    if (rh == ParamValueMissing)
      return ParamValueMissing;

    auto temp = parameters[2];
    if (temp == ParamValueMissing)
      return ParamValueMissing;

    auto rad = parameters[3];

    return feelsLike(wind,rh,temp,rad);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}






void Function_feelsLike::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,const std::vector<double>& extParameters,std::vector<float>& outParameters)
{
  try
  {
    if (inParameters.size() != 4)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double wind = inParameters[0][t];
      double rh = inParameters[1][t];
      double temp = inParameters[2][t];
      double rad = inParameters[3][t];

      outParameters.emplace_back(feelsLike(wind,rh,temp,rad));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void Function_feelsLike::executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,const std::vector<double>& extParameters,std::vector<double>& outParameters)
{
  try
  {
    if (inParameters.size() != 4)
      return;

    uint len = inParameters[0].size();
    outParameters.reserve(len);
    for (uint t=0; t<len; t++)
    {
      double wind = inParameters[0][t];
      double rh = inParameters[1][t];
      double temp = inParameters[2][t];
      double rad = inParameters[3][t];

      outParameters.emplace_back(feelsLike(wind,rh,temp,rad));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




Function* Function_feelsLike::duplicate()
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




void Function_feelsLike::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "Function_feelsLike\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}


}  // namespace Functions
}  // namespace SmartMet
