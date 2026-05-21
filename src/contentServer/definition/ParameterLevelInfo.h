#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Identifies a parameter-level combination using an abstract parameter key.
 *
 *  ParameterLevelInfo combines a parameter key type and key value with a level type
 *  and level value to uniquely address a single grid field dimension. It is used when
 *  querying content without restricting to a specific parameter identification scheme
 *  (FMI id, GRIB1, GRIB2, etc.). */
// ====================================================================================

class ParameterLevelInfo
{
  public:
                         ParameterLevelInfo();
                         ParameterLevelInfo(const ParameterLevelInfo& levelInfo);

                         ParameterLevelInfo(
                             T::ParamKeyType parameterKeyType,
                             T::ParamId parameterKey,
                             T::ParamLevelId parameterLevelId,
                             T::ParamLevel level);

    virtual              ~ParameterLevelInfo();

    ParameterLevelInfo&  operator=(const ParameterLevelInfo& levelInfo);

    ParameterLevelInfo*  duplicate();
    void                 print(std::ostream& stream,uint level,uint optionFlags);

    T::ParamKeyType      mParameterKeyType;   //!< Scheme identifying how mParameterKey should be interpreted.
    T::ParamId           mParameterKey;       //!< Parameter identifier within the scheme given by mParameterKeyType.
    T::ParamLevelId      mParameterLevelId;   //!< Vertical level type identifier.
    T::ParamLevel        mLevel;              //!< Numeric vertical level value.
};



}
}
