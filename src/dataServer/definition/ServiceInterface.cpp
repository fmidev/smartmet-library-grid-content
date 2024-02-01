#include "ServiceInterface.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/ShowFunction.h>
#include <grid-files/grib1/GridDefinition.h>
#include <grid-files/grib2/GridDefinition.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF



namespace SmartMet
{
namespace DataServer
{


ServiceInterface::ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
    mEnabled = true;
    mDebugLog = nullptr;
    mProcessingLog = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServiceInterface::~ServiceInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServiceInterface::getCacheStats(Fmi::Cache::CacheStatistics& statistics) const
{
  FUNCTION_TRACE
  try
  {
    statistics.insert(std::make_pair("Grid-files::GRIB1_coordinate_latlon_cache", GRIB1::latlonCoordinateCache_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB1_coordinate_original_cache", GRIB1::originalCoordinateCache_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB1_coordinate_transform1_cache", GRIB1::transformCache1_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB1_coordinate_transform2_cache", GRIB1::transformCache2_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB1_coordinate_transform3_cache", GRIB1::transformCache3_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB2_coordinate_latlon_cache", GRIB2::latlonCoordinateCache_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB2_coordinate_original_cache", GRIB2::originalCoordinateCache_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB2_coordinate_transform1_cache", GRIB2::transformCache1_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB2_coordinate_transform2_cache", GRIB2::transformCache2_stats));
    statistics.insert(std::make_pair("Grid-files::GRIB2_coordinate_transform3_cache", GRIB2::transformCache3_stats));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





Log* ServiceInterface::getDebugLog()
{
  FUNCTION_TRACE
  try
  {
    return mDebugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




Log* ServiceInterface::getProcessingLog()
{
  FUNCTION_TRACE
  try
  {
    return mProcessingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setProcessingLog(Log *processingLog)
{
  FUNCTION_TRACE
  try
  {
    mProcessingLog = processingLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setDebugLog(Log *debugLog)
{
  FUNCTION_TRACE
  try
  {
    mDebugLog = debugLog;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setEnabled(bool enabled)
{
  FUNCTION_TRACE
  try
  {
    mEnabled = enabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool ServiceInterface::isEnabled()
{
  FUNCTION_TRACE
  try
  {
    return mEnabled;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::shutdown()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void  ServiceInterface::setDem(boost::shared_ptr<Fmi::DEM> dem)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ServiceInterface::setLandCover(boost::shared_ptr<Fmi::LandCover> landCover)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,coordinates);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::getGridLatlonCoordinatesByGeometry(T::SessionId sessionId,T::AttributeList& attributeList,T::GridCoordinates& coordinates)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridLatlonCoordinatesByGeometry(sessionId,attributeList,coordinates);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,GridCoordinates);result %d;time %f;",__FUNCTION__,sessionId,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridData(sessionId,fileId,messageIndex,data);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%lu);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,data.mValues.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridAttributeList(sessionId,fileId,messageIndex,attributeList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,Attribute[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,attributeList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridProperties(T::SessionId sessionId,uint fileId,uint messageIndex,T::PropertySettingVec& propertyList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridProperties(sessionId,fileId,messageIndex,propertyList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,PropertySetting[%ld]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,propertyList.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridFileCount(T::SessionId sessionId,uint& count)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridFileCount(sessionId,count);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u);result %d;time %f;",__FUNCTION__,sessionId,count,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridMessageBytes(sessionId,fileId,messageIndex,messageBytes,messageSections);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,byte[%lu],section[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,messageBytes.size(),messageSections.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getPropertyValuesByCoordinates(T::SessionId sessionId,const char *propertyName,T::Coordinate_vec& latlonCoordinates,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getPropertyValuesByCoordinates(sessionId,propertyName,latlonCoordinates,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%s,Coordinates[%ld],Values[%ld]);result %d;time %f;",__FUNCTION__,sessionId,propertyName,latlonCoordinates.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%f,%f,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%u,%u,%d,%d,%u,%f,%f,%u,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,%f,%f,%u,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,%f,%f,%u,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%u,%u,%d,%u,%u,%d,%u,%u,%d,%s,%u,%u,%f,%f,%u,%u,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime.c_str(),newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,time_t newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%d,%u,%u,%d,%u,%u,%d,%u,%u,%d,%lu,%u,%u,%f,%f,%u,%u,%u,%u,double[%ld],%f);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),value,result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%f,%f,%f,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,%f,%f,%f,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,%f,%f,%f,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,%f,%f,%f,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%u,%f,%f,%f,%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,%u,%f,%f,%f,%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,pointList,areaInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Point[%lu],%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,pointList.size(),areaInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,pointList,areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,Point[%lu],%u,%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,pointList.size(),areaInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,Point[%lu],%u,%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,pointList.size(),areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,Point[%lu],%u,%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,pointList.size(),areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%u,%u,Point[%lu],%u,%u,%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,pointList.size(),areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%u,%u,Point[%lu],%u,%u,%u,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,pointList.size(),areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,polygonPoints,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Point[%lu],%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,polygonPoints.size(),modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPoints,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,Point[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPoints.size(),levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,Point[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,polygonPoints.size(),timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,Point[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPoints.size(),timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%u,Point[%lu],%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,polygonPoints.size(),timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,%u,Point[%lu],%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPoints.size(),timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,polygonPath,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Polygon[%lu],%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,polygonPath.size(),modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPath,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,Polygon[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPath.size(),levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPath,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,Polygon[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,polygonPath.size(),timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPath,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,Polygon[%lu],%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPath.size(),timeInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%u,Polygon[%lu],%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,polygonPath.size(),timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,%u,Polygon[%lu],%d,%d,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPath.size(),timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,modificationOperation,modificationParameters,valueList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%f,%f,%f,%f,%u,double[%ld],GridValue[%u]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,modificationOperation,modificationParameters.size(),valueList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVector(sessionId,fileId,messageIndex,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,timeInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),timeInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,timeInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,timeInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,coordinates,areaInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,Coordinates[%lu],%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,coordinates.size(),areaInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,Coordinates[%lu],Attribute[%u],%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,coordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,Coordinates[%lu],Attribute[%u],%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),coordinateType,coordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,%u,Coordinates[%lu],Attribute[%u],%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,coordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






int ServiceInterface::getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByGeometry(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,Attribute[%u],%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,areaInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%f,%f,%f,%f,%u,%u,double[%ld],ParamValue[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,areaInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,modificationOperation,modificationParameters,valueVector);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%f,%f,%u,%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,modificationOperation,modificationParameters.size(),valueVector.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%d,%d,%d,%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,%d,%d,%d,%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%d,Coordinate[%lu],Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,coordinateType,coordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,%d,Coordinate[%lu],Attribute[%u],%u,double[%ld],value[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,coordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),values.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobands(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByGeometry(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByGrid(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourLowValue[%lu],ContourHighValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues.size(),contourHighValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourLowValue[%lu],ContourHighValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues.size(),contourHighValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolines(sessionId,fileId,messageIndex,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByGeometry(sessionId,fileId,messageIndex,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByGrid(sessionId,fileId,messageIndex,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%lu,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourValue[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%d,ContourValue[%lu],%u,%u,Coordinate[%lu],Attribute[%u],%u,double[%ld],ByteData[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues.size(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),contours.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlines(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlines(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByGeometry(sessionId,fileId,messageIndex,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByGrid(sessionId,fileId,messageIndex,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,Coordinate[%ld],Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId,messageIndex,gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%d,%u,%u,%ld,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%ld,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%ld,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%s,%u,%u,%ld,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime.c_str(),gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%ld,%u,%u,%ld,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%d,Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%s,%d,%u,%u,Coordinate[%ld],Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime.c_str(),newLevel,gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getGridStreamlinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,%u,%u,%u,%u,%u,%u,%u,%ld,%d,%u,%u,Coordinate[%ld],Attribute[%u],%u,double[%ld],streamline[%lu]);result %d;time %f;",__FUNCTION__,sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,gridWidth,gridHeight,gridLatLonCoordinates.size(),attributeList.getLength(),modificationOperation,modificationParameters.size(),streamlines.size(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::getMultipleGridValues(T::SessionId sessionId,uint modificationOperation,double_vec& modificationParameters,T::ValueRecordList& valueRecordList)
{
  FUNCTION_TRACE
  try
  {
    if (!mEnabled)
      return Result::SERVICE_DISABLED;

    unsigned long long timeStart = getTime();
    int result = _getMultipleGridValues(sessionId,modificationOperation,modificationParameters,valueRecordList);
    unsigned long requestTime = getTime() - timeStart;

    PRINT_EVENT_LINE(mProcessingLog,"%s(%llu,%u,double[%ld],ValueRecord[%u]);result %d;time %f;",__FUNCTION__,sessionId,modificationOperation,modificationParameters.size(),valueRecordList.getLength(),result,C_DOUBLE(requestTime) / 1000000);
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





// Notice : If you known the geometryId of the current grid, then you can fetch coordinates
// much faster by using local definitions:
//
//    GRIB2::GridDef_ptr def = Identification::gridDef.getGrib2DefinitionByGeometryId(geometryId);
//    T::Coordinate_vec oridinalCoordinates = def->getGridCoordinates();
//    T::Coordinate_vec latLonCoordinates = def->getGridLatLonCoordinates();

int ServiceInterface::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridLatlonCoordinatesByGeometry(T::SessionId sessionId,T::AttributeList& attributeList,T::GridCoordinates& coordinates)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridProperties(T::SessionId sessionId,uint fileId,uint messageIndex,T::PropertySettingVec& propertyList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridMessageBytes(T::SessionId sessionId,uint fileId,uint messageIndex,std::vector<uchar>& messageBytes,std::vector<uint>& messageSections)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getPropertyValuesByCoordinates(T::SessionId sessionId,const char *propertyName,T::Coordinate_vec& latlonCoordinates,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,value);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTimeUTC,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,value);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,time_t newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue& value)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,origoX,origoY,radius,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,polygonPoints,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,polygonPath,timeInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,valueList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,uint modificationOperation,double_vec& modificationParameters,T::GridValueList& valueList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,timeInterpolationMethod,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,short timeInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,attributeList,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,attributeList,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,coordinateType,coordinates,attributeList,modificationOperation,modificationParameters,values);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ParamValue_vec& values)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourLowValues,contourHighValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourValues,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,contours);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ServiceInterface::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& contours)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridStreamlines(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridStreamlinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridStreamlinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,const std::string& newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTimeUTC,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,time_t newTime,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}




int ServiceInterface::_getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,const std::string& newTime,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  try
  {
    time_t newTimeUTC = utcTimeToTimeT(newTime);
    return _getGridStreamlinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTimeUTC,newLevel,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,modificationOperation,modificationParameters,streamlines);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ServiceInterface::_getGridStreamlinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,time_t newTime,int newLevel,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,uint modificationOperation,double_vec& modificationParameters,T::ByteData_vec& streamlines)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getMultipleGridValues(T::SessionId sessionId,uint modificationOperation,double_vec& modificationParameters,T::ValueRecordList& valueRecordList)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}





int ServiceInterface::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,uint modificationOperation,double_vec& modificationParameters,double_vec& valueVector)
{
  throw Fmi::Exception(BCP,"Implementation required!");
}



}
}

