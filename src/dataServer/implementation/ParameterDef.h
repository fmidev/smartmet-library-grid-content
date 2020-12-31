#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace DataServer
{


class ParameterDef
{
  public:
                                ParameterDef();
                                //ParameterDef(const ParameterDef& def);
                                ParameterDef(const std::string& paramStr);
    virtual                     ~ParameterDef();

    void                        set(const std::string& paramStr);
    std::string                 getName();
    void                        print(std::ostream& stream,uint level,uint optionFlags);

    std::string                 mParameterName;
    std::string                 mProducerName;
    std::string                 mGeometryId;
    std::string                 mLevelId;
    std::string                 mLevel;
    std::string                 mForecastType;
    std::string                 mForecastNumber;
    std::string                 mAreaInterpolationMethod;
    std::string                 mTimeInterpolationMethod;
    std::string                 mLevelInterpolationMethod;
};



typedef std::vector<ParameterDef> ParameterDef_vec;


}  // namespace QueryServer
}  // namespace SmartMet
