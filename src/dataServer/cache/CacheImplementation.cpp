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
    mDataServer = NULL;
    mDataCache = new T::GridData_ptr[mDataCacheSize];
    mDataCacheAccessTime = new time_t[mDataCacheSize];
    for (uint t=0; t<mDataCacheSize; t++)
    {
      mDataCache[t] = NULL;
      mDataCacheAccessTime[t] = 0;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





CacheImplementation::~CacheImplementation()
{
  try
  {
    if (mDataCache != NULL)
    {
      for (uint t=0; t<mDataCacheSize; t++)
      {
        if (mDataCache[t] != NULL)
          delete mDataCache[t];
      }
      delete mDataCache;
      mDataCache = NULL;
    }

    if (mDataCacheAccessTime != NULL)
      delete mDataCacheAccessTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void CacheImplementation::init(ServiceInterface *dataServer)
{
  try
  {
    if (dataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"The 'dataServer' parameter points to NULL!");

    mDataServer = dataServer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,T::AttributeList& attributeList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridAttributeList(sessionId,fileId,messageIndex,attributeList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridCoordinates(sessionId,fileId,messageIndex,coordinateType,coordinates);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,T::GridData& data)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    for (uint t=0; t<mDataCacheSize; t++)
    {
      T::GridData *dt = mDataCache[t];
      if (dt != NULL  &&  dt->mFileId == fileId  &&  dt->mMessageIndex == messageIndex)
      {
        mDataCacheAccessTime[t] = time(0);
        data = *dt;
        //printf("** DATA FROM CACHE %u\n",fileId);
        return Result::OK;
      }
    }

    int result = mDataServer->getGridData(sessionId,fileId,messageIndex,data);
    if (result == Result::OK)
    {
      time_t oldest = time(0);
      uint idx = 0;
      for (uint t=0; t<mDataCacheSize; t++)
      {
        if (mDataCacheAccessTime[t] < oldest)
        {
          oldest = mDataCacheAccessTime[t];
          idx = t;
        }
      }

      if (mDataCache[idx] == NULL)
        mDataCache[idx] = new T::GridData(data);
      else
        *mDataCache[idx] = data;

      //printf("** DATA TO CACHE %u\n",fileId);
      mDataCacheAccessTime[idx] = time(0);
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridFileCount(T::SessionId sessionId,uint& count)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridFileCount(sessionId,count);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    if (mPointValueCache.getValue(fileId,messageIndex,coordinateType,x,y,interpolationMethod,value))
    {
      //printf("Value from cache %f,%f %f\n",x,y,value);
      return Result::OK;
    }

    int result = mDataServer->getGridValueByPoint(sessionId,fileId,messageIndex,coordinateType,x,y,interpolationMethod,value);
    if (result == Result::OK)
      mPointValueCache.addValue(fileId,messageIndex,coordinateType,x,y,interpolationMethod,value);

    //printf("Value to cache %f,%f %f\n",x,y,value);

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getMultipleGridValues(T::SessionId sessionId,T::ValueRecordList& valueRecordList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getMultipleGridValues(sessionId,valueRecordList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByCircle(sessionId,fileId,messageIndex,coordinateType,origoX,origoY,radius,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,T::InterpolationMethod interpolationMethod,T::GridValueList& valueList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPointList(sessionId,fileId,messageIndex,coordinateType,pointList,interpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygon(sessionId,fileId,messageIndex,coordinateType,polygonPoints,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,coordinateType,polygonPath,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByRectangle(sessionId,fileId,messageIndex,coordinateType,x1,y1,x2,y2,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,T::ParamValue_vec& values)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVector(sessionId,fileId,messageIndex,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int CacheImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,T::InterpolationMethod interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    if (mDataServer == NULL)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}

