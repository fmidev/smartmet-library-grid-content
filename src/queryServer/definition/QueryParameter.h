#pragma once

#include "ParameterValues.h"
#include <grid-files/common/AttributeList.h>


namespace SmartMet
{
namespace QueryServer
{

typedef std::pair<uint,std::string> FunctionParam;
typedef std::vector<FunctionParam> FunctionParam_vec;
typedef std::vector<uint> Color_vec;


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
    void                        updateForecastTimeList(std::set<time_t>& mForecastTimeList);

    uint                        mId;
    uint                        mAlternativeParamId;   // This parameter is an alternative for parameter which id is defined here.
    uchar                       mType;
    uchar                       mLocationType;
    std::string                 mParam;
    std::string                 mOrigParam;
    std::string                 mSymbolicName;
    T::ParamKeyType             mParameterKeyType;
    T::ParamId                  mParameterKey;
    std::string                 mProducerName;
    T::GeometryId               mGeometryId;
    T::ParamLevelId             mParameterLevelId;
    T::ParamLevel               mParameterLevel;
    T::ForecastType             mForecastType;
    T::ForecastNumber           mForecastNumber;
    short                       mAreaInterpolationMethod;
    short                       mTimeInterpolationMethod;
    short                       mLevelInterpolationMethod;
    T::ParamValue_vec           mContourLowValues;
    T::ParamValue_vec           mContourHighValues;
    Color_vec                   mContourColors;
    uint                        mProducerId;
    ulonglong                   mGenerationFlags;
    short                       mPrecision;
    bool                        mTemporary;
    std::string                 mFunction;
    FunctionParam_vec           mFunctionParams;
    uint                        mTimestepsBefore;
    uint                        mTimestepsAfter;
    uint                        mTimestepSizeInMinutes;
    uint                        mFlags;
    ParameterValues_sptr_vec    mValueList;
    T::Coordinate_vec           mCoordinates;
    T::AttributeList            mAttributeList;


    class Type
    {
      public:
        static const uchar PointValues    = 0;
        static const uchar Isoline        = 1;
        static const uchar Isoband        = 2;
        static const uchar Vector         = 3;
        static const uchar GridFile       = 4;
        static const uchar StreamLine     = 5;
    };

    class LocationType
    {
      public:
        static const uchar Point      = 0;  // mAreaCoordinates[0][0]   => Singe x,y -coordinate
        static const uchar Polygon    = 1;  // mAreaCoordinates[0 - N]  => 1-N polygons
        static const uchar Path       = 2;  // mAreaCoordinates[0]      => List of x,y -coordinates
        static const uchar Circle     = 3;  // mAreaCoordinates[0][0] + radius
        static const uchar Grid       = 4;  // mAttributeList : "grid.width", "grid.height" ; mAreaCoordinates[0] => Grid coordinates
        static const uchar Geometry   = 5;  // mAttributeList : "grid.geometryId","grid.geometryString"
    };

    class Flags
    {
      public:
        static const uint  PressureLevelInterpolation   = 1 << 0;
        static const uint  HeightLevelInterpolation     = 1 << 1;
        static const uint  ReturnCoordinates            = 1 << 2;
        static const uint  NoReturnValues               = 1 << 3;
        static const uint  SameAnalysisTime             = 1 << 4;
        static const uint  AlternativeParameter         = 1 << 5;
        static const uint  InvisibleParameter           = 1 << 6;
        static const uint  OriginalCoordinates          = 1 << 7;
        static const uint  AggregationParameter         = 1 << 8;
        static const uint  InternalAggregationParameter = 1 << 9;
    };
};


typedef std::vector<QueryParameter> QueryParameter_vec;

}  // namespace QueryServer
}  // namespace SmartMet

