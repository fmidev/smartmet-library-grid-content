#pragma once

#include "ParameterValues.h"
#include <grid-files/common/AttributeList.h>


namespace SmartMet
{
namespace QueryServer
{

typedef std::pair<uint,std::string> FunctionParam;      //!< A single function argument: (type, value-string) pair.
typedef std::vector<FunctionParam> FunctionParam_vec;   //!< Ordered list of function arguments.
typedef std::vector<uint> Color_vec;                    //!< List of RGBA colour values used for isoband rendering.

// ====================================================================================
/*! \brief Specification for a single named parameter within a Query.
 *
 *  Holds all attributes that identify and constrain one parameter retrieval:
 *  name, level, forecast type, interpolation methods, optional function expression
 *  and its arguments, aggregation window, and the resulting ParameterValues list
 *  populated after execution. */
// ====================================================================================

class QueryParameter
{
  public:
                                QueryParameter();
                                //QueryParameter(const QueryParameter& queryParameter);
    virtual                     ~QueryParameter();

    std::size_t                 getHash();
    QueryParameter*             duplicate();
    void                        print(std::ostream& stream,uint level,uint optionFlags);

    void                        getValueListSize(int& columns,int& rows);
    T::ParamValue               getValueListValue(int col,int row);
    void                        getValueListValuesByColumnRange(int startCol,int endCol,int row,std::vector<T::ParamValue>& values);
    void                        getValueListValuesByRowRange(int col,int startRow,int endRow,std::vector<T::ParamValue>& values);
    T::GridValue*               getValueListRecord(int col,int row);
    void                        getValueVectorsByRowRange(int startRow,int endRow,std::vector<std::vector<T::ParamValue>>& valueVectors);
    void                        removeAggregationValues();
    void                        removeInternalAggregationValues();
    void                        removeTimes(std::set<time_t>& times);
    void                        updateForecastTimeList(std::set<time_t>& mForecastTimeList);

    uint                        mId;                       //!< Internal identifier within the owning Query.
    uint                        mAlternativeParamId;       //!< Id of the primary parameter this one is an alternative for.
    uchar                       mType;                     //!< Output type from Type constants (PointValues, Isoline, …).
    uchar                       mLocationType;             //!< Spatial specification type from LocationType constants.
    std::string                 mParam;                    //!< Resolved parameter expression string used for lookup.
    std::string                 mOrigParam;                //!< Original parameter string as received from the caller.
    std::string                 mSymbolicName;             //!< Optional symbolic name for referencing this parameter in functions.
    T::ParamKeyType             mParameterKeyType;         //!< Key encoding resolved from mParam.
    T::ParamId                  mParameterKey;             //!< Concrete parameter key resolved from mParam.
    std::string                 mProducerName;             //!< Producer name resolved from mParam, empty = any.
    T::GeometryId               mGeometryId;               //!< Required geometry identifier, 0 = any.
    T::ParamLevelId             mParameterLevelId;         //!< Required level type, 0 = any.
    T::ParamLevel               mParameterLevel;           //!< Required level value, 0 = any.
    T::ForecastType             mForecastType;             //!< Required forecast type, -1 = any.
    T::ForecastNumber           mForecastNumber;           //!< Required forecast (ensemble) number, -1 = any.
    short                       mAreaInterpolationMethod;  //!< Spatial interpolation method override, -1 = default.
    short                       mTimeInterpolationMethod;  //!< Time interpolation method override, -1 = default.
    short                       mLevelInterpolationMethod; //!< Level interpolation method override, -1 = default.
    T::ParamValue_vec           mContourLowValues;         //!< Lower threshold values for isoband rendering.
    T::ParamValue_vec           mContourHighValues;        //!< Upper threshold values for isoband rendering.
    Color_vec                   mContourColors;            //!< Fill colours for isoband rendering.
    T::ProducerId               mProducerId;               //!< Resolved producer identifier (populated during execution).
    UInt64                      mGenerationFlags;          //!< Generation-selection flags for this parameter.
    short                       mPrecision;                //!< Decimal precision for formatted output, -1 = default.
    bool                        mTemporary;                //!< When true, this parameter is removed after function evaluation.
    std::string                 mFunction;                 //!< Name of the function to apply to the retrieved values.
    FunctionParam_vec           mFunctionParams;           //!< Arguments passed to mFunction.
    uint                        mTimestepsBefore;          //!< Number of extra time steps before each requested time for aggregation.
    uint                        mTimestepsAfter;           //!< Number of extra time steps after each requested time for aggregation.
    uint                        mTimestepSizeInMinutes;    //!< Step size in minutes for aggregation window expansion.
    uint                        mFlags;                    //!< Bitmask from Flags controlling retrieval behaviour.
    ParameterValues_sptr_vec    mValueList;                //!< Result values populated by the QueryServer after execution.
    std::vector<T::ParamLevel>  mParameterLevels;          //!< Explicit list of levels to retrieve (vertical profile queries).
    T::Coordinate_vec           mCoordinates;              //!< Per-parameter coordinate overrides, if different from the Query.
    T::AttributeList            mAttributeList;            //!< Additional key-value attributes for this parameter.


    // ====================================================================================
    /*! \brief Output type constants for mType. */
    // ====================================================================================
    class Type
    {
      public:
        static const uchar PointValues    = 0;  //!< Return values at individual points.
        static const uchar Isoline        = 1;  //!< Return isoline geometry.
        static const uchar Isoband        = 2;  //!< Return isoband geometry.
        static const uchar Vector         = 3;  //!< Return vector (wind) field data.
        static const uchar GridFile       = 4;  //!< Return raw grid file content.
        static const uchar StreamLine     = 5;  //!< Return streamline geometry.
        static const uchar VerticalVector = 6;  //!< Return vertical profile vector data.
    };

    // ====================================================================================
    /*! \brief Spatial specification type constants for mLocationType. */
    // ====================================================================================
    class LocationType
    {
      public:
        static const uchar Point      = 0;  //!< Single x,y coordinate from mAreaCoordinates[0][0].
        static const uchar Polygon    = 1;  //!< One or more polygon rings from mAreaCoordinates[0..N].
        static const uchar Path       = 2;  //!< Ordered list of x,y coordinates from mAreaCoordinates[0].
        static const uchar Circle     = 3;  //!< Centre point from mAreaCoordinates[0][0] plus mRadius.
        static const uchar Grid       = 4;  //!< Grid defined by "grid.width"/"grid.height" attributes and corner coordinates.
        static const uchar Geometry   = 5;  //!< Geometry from "grid.geometryId" or "grid.geometryString" attributes.
    };

    // ====================================================================================
    /*! \brief Bit-flag constants for mFlags. */
    // ====================================================================================
    class Flags
    {
      public:
        static const uint  PressureLevels               = 1 << 0;   //!< Levels are interpreted as pressure levels.
        static const uint  MetricLevels                 = 1 << 1;   //!< Levels are interpreted as metres above ground.
        static const uint  ReturnCoordinates            = 1 << 2;   //!< Include coordinates in the result value list.
        static const uint  NoReturnValues               = 1 << 3;   //!< Suppress value data in the result (geometry only).
        static const uint  SameAnalysisTime             = 1 << 4;   //!< All time steps must share the same analysis time.
        static const uint  AlternativeParameter         = 1 << 5;   //!< This parameter is an alternative for mAlternativeParamId.
        static const uint  InvisibleParameter           = 1 << 6;   //!< Parameter is used internally and not returned to the caller.
        static const uint  OriginalCoordinates          = 1 << 7;   //!< Return values in the original grid coordinate system.
        static const uint  AggregationParameter         = 1 << 8;   //!< Parameter is used for aggregation purposes.
        static const uint  InternalAggregationParameter = 1 << 9;   //!< Parameter is used for internal aggregation and removed afterwards.
    };
};


typedef std::vector<QueryParameter> QueryParameter_vec;  //!< Ordered list of QueryParameter specifications.

}  // namespace QueryServer
}  // namespace SmartMet

