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
      delete mDataCache;
      mDataCache = nullptr;
    }

    if (mDataCacheAccessTime != nullptr)
      delete mDataCacheAccessTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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





int CacheImplementation::_getGridAttributeList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::AttributeList& attributeList)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridAttributeList(sessionId,fileId,messageIndex,flags,attributeList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridCoordinates(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,T::GridCoordinates& coordinates)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridCoordinates(sessionId,fileId,messageIndex,flags,coordinateType,coordinates);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridData(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::GridData& data)
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

    int result = mDataServer->getGridData(sessionId,fileId,messageIndex,flags,data);
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





int CacheImplementation::_getGridValueByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,short interpolationMethod,T::ParamValue& value)
{
  try
  {
    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    if (mPointValueCache.getValue(fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod,value))
    {
      //printf("Value from cache %f,%f %f\n",x,y,value);
      return Result::OK;
    }

    int result = mDataServer->getGridValueByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod,value);
    if (result == Result::OK)
      mPointValueCache.addValue(fileId,messageIndex,flags,coordinateType,x,y,interpolationMethod,value);

    //printf("Value to cache %f,%f %f\n",x,y,value);

    return result;
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





int CacheImplementation::_getGridValueListByCircle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double origoX,double origoY,double radius,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByCircle(sessionId,fileId,messageIndex,flags,coordinateType,origoX,origoY,radius,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPointList(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& pointList,short interpolationMethod,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPointList(sessionId,fileId,messageIndex,flags,coordinateType,pointList,interpolationMethod,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPolygon(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<T::Coordinate>& polygonPoints,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygon(sessionId,fileId,messageIndex,flags,coordinateType,polygonPoints,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByPolygonPath(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,std::vector<std::vector<T::Coordinate>>& polygonPath,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByPolygonPath(sessionId,fileId,messageIndex,flags,coordinateType,polygonPath,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueListByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x1,double y1,double x2,double y2,T::GridValueList& valueList)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueListByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,x1,y1,x2,y2,valueList);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVector(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVector(sessionId,fileId,messageIndex,flags,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByCoordinateList(T::SessionId sessionId,uint fileId,uint messageIndex,T::CoordinateType coordinateType,std::vector<T::Coordinate>& coordinates,short interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByCoordinateList(sessionId,fileId,messageIndex,coordinateType,coordinates,interpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByRectangle(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,uint columns,uint rows,double x,double y,double xStep,double yStep,short interpolationMethod,T::ParamValue_vec& values)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByRectangle(sessionId,fileId,messageIndex,flags,coordinateType,columns,rows,x,y,xStep,yStep,interpolationMethod,values);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int CacheImplementation::_getGridValueVectorByPoint(T::SessionId sessionId,uint fileId,uint messageIndex,uint flags,T::CoordinateType coordinateType,double x,double y,uint vectorType,double_vec& valueVector)
{
  try
  {
    // ToDo : Implement caching functionality

    if (mDataServer == nullptr)
      throw SmartMet::Spine::Exception(BCP,"Service not initialized!");

    return mDataServer->getGridValueVectorByPoint(sessionId,fileId,messageIndex,flags,coordinateType,x,y,vectorType,valueVector);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}

