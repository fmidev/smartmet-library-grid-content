#include "CacheImplementation.h"


namespace SmartMet
{
namespace DataServer
{




CacheImplementation::CacheImplementation()
{
  try
  {
    mDataCacheSize = 1000;
    mDataServer = nullptr;
    mDataCache = new T::GridData_ptr[mDataCacheSize];
    mDataCacheAccessTime = new time_t[mDataCacheSize];
    for (uint t=0; t<mDataCacheSize; t++)
    {
      mDataCache[t] = nullptr;
      mDataCacheAccessTime[t] = 0;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





CacheImplementation::~CacheImplementation()
{
  try
  {
    if (mDataCache != nullptr)
    {
      for (uint t=0; t<mDataCacheSize; t++)
      {
        if (mDataCache[t] != nullptr)
          delete mDataCache[t];
      }
      delete[] mDataCache;
      mDataCache = nullptr;
    }

    if (mDataCacheAccessTime != nullptr)
      delete[] mDataCacheAccessTime;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void CacheImplementation::init(ServiceInterface *dataServer)
{
  try
  {
    if (dataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"The 'dataServer' parameter points to nullptr!");

    mDataServer = dataServer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridAttributeList(sessionId,fileId,messageIndex,attributeList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,coordinates);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    for (uint t=0; t<mDataCacheSize; t++)
    {
      T::GridData *dt = mDataCache[t];
      if (dt != nullptr  &&  dt->mFileId == fileId  &&  dt->mMessageIndex == messageIndex)
      {
        mDataCacheAccessTime[t] = time(nullptr);
        data = *dt;
        //printf("** DATA FROM CACHE %u\n",fileId);
        return Result::OK;
      }
    }

    int result = mDataServer->getGridData(sessionId,fileId,messageIndex,data);
    if (result == Result::OK)
    {
      time_t oldest = time(nullptr);
      uint idx = 0;
      for (uint t=0; t<mDataCacheSize; t++)
      {
        if (mDataCacheAccessTime[t] < oldest)
        {
          oldest = mDataCacheAccessTime[t];
          idx = t;
        }
      }

      if (mDataCache[idx] == nullptr)
        mDataCache[idx] = new T::GridData(data);
      else
        *mDataCache[idx] = data;

      //printf("** DATA TO CACHE %u\n",fileId);
      mDataCacheAccessTime[idx] = time(nullptr);
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridFileCount(sessionId,count);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,T::ParamValue& value)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    if (mPointValueCache.getValue(fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,value))
    {
      //printf("Value from cache %f,%f %f\n",x,y,value);
      return Result::OK;
    }

    int result = mDataServer->getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,value);
    if (result == Result::OK)
      mPointValueCache.addValue(fileId,messageIndex,coordinateType,x,y,areaInterpolationMethod,value);

    //printf("Value to cache %f,%f %f\n",x,y,value);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueByLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueByLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,newLevel,coordinateType,x,y,areaInterpolationMethod,levelInterpolationMethod,value);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueByTimeAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,T::ParamValue& value)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueByTimeAndPoint(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,value);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueByTimeLevelAndPoint(T::SessionId sessionId,uint fileId1,uint messageIndex1,int level1,uint fileId2,uint messageIndex2,int level2,uint fileId3,uint messageIndex3,int level3,uint fileId4,uint messageIndex4,int level4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double x,double y,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue& value)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueByTimeLevelAndPoint(sessionId,fileId1,messageIndex1,level1,fileId2,messageIndex2,level2,fileId3,messageIndex3,level3,fileId4,messageIndex4,level4,newTime,newLevel,coordinateType,x,y,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,value);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getMultipleGridValues(sessionId,valueRecordList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,origoX,origoY,radius,timeInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,origoX,origoY,radius,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeLevelAndCircle(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,double origoX,double origoY,double radius,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeLevelAndCircle(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,origoX,origoY,radius,timeInterpolationMethod,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,pointList,areaInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,pointList,areaInterpolationMethod,levelInterpolationMethod,valueList);;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeLevelAndPointList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeLevelAndPointList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,pointList,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,polygonPoints,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPoints,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPoints,timeInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeLevelAndPolygon(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeLevelAndPolygon(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPoints,timeInterpolationMethod,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,polygonPath,timeInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,polygonPath,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,polygonPath,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByTimeLevelAndPolygonPath(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,short timeInterpolationMethod,short levelInterpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByTimeLevelAndPolygonPath(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,polygonPath,timeInterpolationMethod,levelInterpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVector(sessionId,fileId,messageIndex,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,levelInterpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,short timeInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,timeInterpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,coordinateType,coordinates,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByTimeAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTimeAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,coordinateType,coordinates,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getGridValueVectorByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,coordinates,areaInterpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByGeometry(sessionId,fileId,messageIndex,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short areaInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,areaInterpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,vectorType,valueVector);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,short areaInterpolationMethod,short timeInterpolationMethod,short levelInterpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,areaInterpolationMethod,timeInterpolationMethod,levelInterpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




int CacheImplementation::_getGridValueVectorByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByTimeLevelAndCoordinateList(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,T::AttributeList& attributeList,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByTimeLevelAndCoordinateList(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,coordinateType,coordinates,attributeList,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobands(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobands(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByGeometry(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByGrid(sessionId,fileId,messageIndex,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsobandsByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourLowValues,T::ParamValue_vec& contourHighValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsobandsByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourLowValues,contourHighValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolines(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolines(sessionId,fileId,messageIndex,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByGeometry(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByGeometry(sessionId,fileId,messageIndex,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByGrid(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByGrid(sessionId,fileId,messageIndex,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTime(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTime(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTimeAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTimeAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTimeAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,std::string newTime,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTimeAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,newTime,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTimeAndLevel(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTimeAndLevel(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTimeLevelAndGeometry(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTimeLevelAndGeometry(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridIsolinesByTimeLevelAndGrid(T::SessionId sessionId,uint fileId1,uint messageIndex1,uint fileId2,uint messageIndex2,uint fileId3,uint messageIndex3,uint fileId4,uint messageIndex4,std::string newTime,int newLevel,T::ParamValue_vec& contourValues,uint gridWidth,uint gridHeight,std::vector<T::Coordinate>& gridLatLonCoordinates,T::AttributeList& attributeList,T::ByteData_vec& contours)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridIsolinesByTimeLevelAndGrid(sessionId,fileId1,messageIndex1,fileId2,messageIndex2,fileId3,messageIndex3,fileId4,messageIndex4,newTime,newLevel,contourValues,gridWidth,gridHeight,gridLatLonCoordinates,attributeList,contours);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}

