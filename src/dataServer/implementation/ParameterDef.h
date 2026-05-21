#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace DataServer
{


// ====================================================================================
/*! \brief Parsed parameter definition used to drive a DataServer data retrieval.
 *
 *  All fields are strings parsed from a colon-separated parameter specification
 *  (e.g. "temperature:ECMWF:1001:2:10:-1:-1:bi:li:li").  Empty strings mean
 *  "any" or "use default". */
// ====================================================================================

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

    std::string                 mParameterName;            //!< Parameter short name or identifier string.
    std::string                 mProducerName;             //!< Producer name filter (empty = any).
    std::string                 mGeometryId;               //!< Geometry identifier filter (empty = any).
    std::string                 mLevelId;                  //!< Level type identifier filter.
    std::string                 mLevel;                    //!< Vertical level value filter.
    std::string                 mForecastType;             //!< Forecast type filter (empty = any).
    std::string                 mForecastNumber;           //!< Ensemble member number filter (empty = any).
    std::string                 mAreaInterpolationMethod;  //!< Spatial interpolation method name.
    std::string                 mTimeInterpolationMethod;  //!< Temporal interpolation method name.
    std::string                 mLevelInterpolationMethod; //!< Vertical interpolation method name.
};



typedef std::vector<ParameterDef> ParameterDef_vec;  //!< Vector of ParameterDef objects.


}  // namespace QueryServer
}  // namespace SmartMet
