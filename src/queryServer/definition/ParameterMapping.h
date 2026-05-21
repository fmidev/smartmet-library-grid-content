#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace QueryServer
{

// ====================================================================================
/*! \brief Maps a high-level named parameter to its low-level GRIB/NetCDF identifier.
 *
 *  Each record associates a producer name and human-readable parameter name with the
 *  concrete key type and key value used to locate data in the grid file store.
 *  Optional interpolation method overrides and conversion function names allow
 *  per-mapping customisation of retrieval and unit conversion. */
// ====================================================================================

class ParameterMapping
{
  public:
                                ParameterMapping();
                                //ParameterMapping(const ParameterMapping& mapping);
    virtual                     ~ParameterMapping();

    virtual std::size_t         getHash();
    virtual std::size_t         getKeyHash();

    void                        print(std::ostream& stream,uint level,uint optionFlags);

    std::string                 mProducerName;              //!< Name of the producer this mapping applies to (empty = any).
    std::string                 mParameterName;             //!< Human-readable parameter name used in queries.
    T::ParamKeyType             mParameterKeyType;          //!< Key encoding (GRIB1, GRIB2, netCDF variable name, …).
    T::ParamId                  mParameterKey;              //!< Concrete parameter key in the encoding given by mParameterKeyType.
    std::size_t                 mParameterKeyHash;          //!< Pre-computed hash of mParameterKey for fast comparison.
    T::GeometryId               mGeometryId;                //!< Required geometry, or 0 to match any geometry.
    T::ParamLevelId             mParameterLevelId;          //!< Required level type identifier, or 0 to match any.
    T::ParamLevel               mParameterLevel;            //!< Required level value, or 0 to match any.
    short                       mAreaInterpolationMethod;   //!< Default spatial interpolation method for this mapping.
    short                       mTimeInterpolationMethod;   //!< Default time interpolation method for this mapping.
    short                       mLevelInterpolationMethod;  //!< Default level interpolation method for this mapping.
    uint                        mGroupFlags;                //!< Bitmask from GroupFlags classifying this mapping.
    bool                        mSearchEnabled;             //!< When false, the mapping is skipped in automated searches.
    bool                        mIgnore;                    //!< When true, this mapping is excluded from all lookups.
    std::string                 mConversionFunction;        //!< Lua/built-in function name for forward unit conversion.
    std::string                 mReverseConversionFunction; //!< Lua/built-in function name for reverse unit conversion.
    short                       mDefaultPrecision;          //!< Default decimal precision for formatted output.


    // ====================================================================================
    /*! \brief Bit-flag constants for mGroupFlags. */
    // ====================================================================================
    class GroupFlags
    {
      public:
        static const uint       climatology = 1 << 0;  //!< Mapping belongs to the climatology group.
        static const uint       global = 1 << 1;        //!< Mapping covers a global grid.
        static const uint       bias = 1 << 2;          //!< Mapping represents a bias-corrected field.
    };
};





typedef std::vector<ParameterMapping> ParameterMapping_vec;  //!< Ordered list of ParameterMapping records.


}  // namespace QueryServer
}  // namespace SmartMet
