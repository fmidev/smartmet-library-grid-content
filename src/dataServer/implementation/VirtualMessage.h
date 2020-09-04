#pragma once

#include "../../lua/LuaFileCollection.h"
#include "../../functions/FunctionCollection.h"
#include "../../contentServer/definition/ContentInfo.h"
#include "ParameterDef.h"

#include <grid-files/grid/Message.h>
#include <grid-files/grid/GridFile.h>
#include <grid-files/grid/VirtualGridFile.h>


namespace SmartMet
{
namespace GRID
{


typedef std::pair<GRID::GridFile_sptr,uint> SourceMessage;



class VirtualMessage : public Message
{
  public:

                                    VirtualMessage(GRID::VirtualGridFile *gridFile,DataServer::ParameterDef& paramDef,std::vector<SourceMessage>& sourceMessages);
    virtual                         ~VirtualMessage();

    virtual void                    setFunction(Functions::FunctionCollection *functionCollection,Lua::LuaFileCollection *luaFileCollection,std::string functionName,uint functionCallMethod,std::vector<double>& functionParameters);

    virtual uint                    getFileId() const;
    virtual void                    getAttributeList(std::string prefix,T::AttributeList& attributeList) const;
    virtual T::TimeString           getForecastTime() const;
    virtual time_t                  getForecastTimeT() const;
    virtual short                   getForecastType() const;
    virtual short                   getForecastNumber() const;

    virtual T::GeometryId           getGridGeometryId() const;
    virtual T::Coordinate_svec      getGridCoordinates() const;
    virtual T::Coordinate_svec      getGridLatLonCoordinates() const;
    virtual T::Dimensions           getGridDimensions() const;
    virtual T::Hash                 getGridHash() const;
    virtual bool                    getGridLatLonCoordinatesByGridPoint(uint grid_i,uint grid_j,double& lat,double& lon) const;
    virtual bool                    getGridLatLonCoordinatesByGridPosition(double grid_i,double grid_j,double& lat,double& lon) const;
    virtual bool                    getGridLatLonCoordinatesByOriginalCoordinates(double x,double y,double& lat,double& lon) const;
    virtual bool                    getGridOriginalCoordinatesByGridPoint(uint grid_i,uint grid_j,double& x,double& y) const;
    virtual bool                    getGridOriginalCoordinatesByGridPosition(double grid_i,double grid_j,double& x,double& y) const;
    virtual bool                    getGridOriginalCoordinatesByLatLonCoordinates(double lat,double lon,double& x,double& y) const;
    virtual T::GridLayout           getGridLayout() const;
    virtual std::size_t             getGridOriginalColumnCount(std::size_t row) const;
    virtual std::size_t             getGridOriginalColumnCount() const;
    virtual std::size_t             getGridOriginalRowCount() const;
    virtual std::size_t             getGridOriginalValueCount() const;
    virtual int                     getGridOriginalValueIndex(uint grid_i,uint grid_j) const;
    virtual bool                    getGridPointByLatLonCoordinates(double lat,double lon,double& grid_i,double& grid_j) const;
    virtual bool                    getGridPointByOriginalCoordinates(double x,double y,double& grid_i,double& grid_j) const;
    virtual T::GridProjection       getGridProjection() const;
    virtual void                    getGridProjectionAttributes(std::string prefix,T::AttributeList& attributeList) const;
    virtual std::string             getGridProjectionString() const;

    virtual void                    getGridValueByPoint(T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value);
    virtual void                    getGridValueListByCircle(T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList);
    virtual void                    getGridValueListByPointList(T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList);
    virtual void                    getGridValueListByPolygon(T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList);
    virtual void                    getGridValueListByPolygonPath(T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList);
    virtual void                    getGridValueListByRectangle(T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,bool gridRectangle,T::GridValueList& valueList);
    virtual void                    getGridValueVector(T::ParamValue_vec& values) const;
    virtual void                    getGridOriginalValueVector(T::ParamValue_vec& values) const;
    virtual T::ParamLevel           getGridParameterLevel() const;
    virtual T::ParamLevelId         getGridParameterLevelId() const;
    virtual std::string             getGridParameterLevelIdString() const;
    virtual void                    getGridMinAndMaxValues(T::ParamValue& minValue,T::ParamValue& maxValue) const;

    virtual T::ParamValue           getGridValueByGridPoint(uint grid_i,uint grid_j) const;
    virtual T::ParamValue           getGridValueByGridPoint_byInterpolation(double grid_i,double grid_j,short interpolationMethod) const;
    virtual T::ParamValue           getGridValueByGridPoint_nearest(double grid_i,double grid_j) const;
    virtual T::ParamValue           getGridValueByGridPoint_linearInterpolation(double grid_i,double grid_j) const;
    virtual T::ParamValue           getGridValueByLatLonCoordinate(double lat,double lon,short interpolationMethod) const;
    virtual T::ParamValue           getGridValueByOriginalGridPoint(uint grid_i,uint grid_j) const;
    virtual void                    getGridValueVectorByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValue_vec& gridPointValues) const;
    virtual void                    getParameterValuesByRectangle(uint grid_i_start,uint grid_j_start,uint grid_i_end,uint grid_j_end,T::GridPointValueList& gridPointValues) const;
    virtual T::TimeString           getReferenceTime() const;
    virtual T::SpatialRef*          getSpatialReference() const;
    virtual std::string             getWKT() const;
    virtual bool                    reverseXDirection() const;
    virtual bool                    reverseYDirection() const;

    virtual void                    setContentInfo(T::ContentInfo& contentInfo);
    virtual T::ContentInfo*         getContentInfo();

    virtual bool                    isGridGlobal() const;

    virtual void                    print(std::ostream& stream,uint level,uint optionFlags) const;

  protected:

    virtual void                    initMessagePtrs() const;

    virtual void                    executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<float>>& inParameters,std::vector<float>& outParameters) const;
    virtual void                    executeFunctionCall9(uint columns,uint rows,std::vector<std::vector<double>>& inParameters,std::vector<double>& outParameters) const;
    virtual void                    executeFunctionCall9(std::vector<T::GridValueList>& inValueList,T::GridValueList& outValueList) const;

    GRID::VirtualGridFile*          mVirtualGridFile;
    mutable GRID::MessagePtr_vec    mMessageList;
    Functions::FunctionCollection*  mFunctionCollection;
    Lua::LuaFileCollection*         mLuaFileCollection;
    std::string                     mFunctionName;
    uint                            mFunctionCallMethod;
    std::vector<double>             mFunctionParameters;
    DataServer::ParameterDef        mParamDef;
    std::vector<SourceMessage>      mSourceMessages;
    T::ContentInfo                  mContentInfo;
};


}  // namespace GRID
}  // namespace SmartMet

