#include "ServiceImplementation.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace QueryServer
{


ServiceImplementation::ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = NULL;
    mDataServerPtr = NULL;
    mFunctionParamId = 0;
    mProducerFileModificationTime = 0;
    mLastConfiguratonCheck = 0;
    mCheckInterval = 5;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ServiceImplementation::~ServiceImplementation()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::init(
    ContentServer::ServiceInterface *contentServerPtr,
    DataServer::ServiceInterface *dataServerPtr,
    std::string gridConfigFile,
    string_vec& parameterMappingFiles,
    string_vec& aliasFiles,
    std::string producerFile,
    std::string producerAliasFile,
    string_vec& luaFiles)
{
  FUNCTION_TRACE
  try
  {
    mContentServerPtr = contentServerPtr;
    mDataServerPtr = dataServerPtr;

    mGridConfigFile = gridConfigFile;
    SmartMet::Identification::gridDef.init(mGridConfigFile.c_str());

    mProducerFile = producerFile;
    loadProducerFile();

    mProducerAliasFile.init(producerAliasFile);

    mAliasFileCollection.init(aliasFiles);
    mLuaFileCollection.init(luaFiles);

    mParameterMappingFiles = parameterMappingFiles;

    for (auto it = mParameterMappingFiles.begin(); it != mParameterMappingFiles.end(); ++it)
    {
      ParameterMappingFile mapping(*it);
      mParameterMappings.push_back(mapping);
    }

    for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
    {
      it->init();
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::shutdown()
{
  FUNCTION_TRACE
  try
  {
    exit(0);
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





int ServiceImplementation::_executeQuery(T::SessionId sessionId,Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Checking if the configuration files have changed. If so, the
    // changes will be loaded automatically.

    checkConfigurationUpdates();

    //query.print(std::cout,0,0);

    if (mDebugLog != NULL)
    {
      std::stringstream stream;
      query.print(stream,0,0);
      PRINT_DATA(mDebugLog,"%s",stream.str().c_str());
    }

    int result = 0;
    if ((query.mFlags & QF_TIME_RANGE_QUERY) != 0)
      result = executeTimeRangeQuery(query);
    else
      result = executeTimeStepQuery(query);

    return result;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::_getProducerList(T::SessionId sessionId,string_vec& producerList)
{
  FUNCTION_TRACE
  try
  {
    std::string prev;
    for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
    {
      if (it->first != prev)
        producerList.push_back(it->first);

      prev = it->first;
    }
    return QueryServer::Result::OK;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::loadProducerFile()
{
  FUNCTION_TRACE
  try
  {
    FILE *file = fopen(mProducerFile.c_str(),"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the producer file!");
      exception.addParameter("Filename",mProducerFile);
      throw exception;
    }

    mProducerList.clear();

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL  &&  st[0] != '#')
      {
        bool ind = false;
        char *field[100];
        uint c = 1;
        field[0] = st;
        char *p = st;
        while (*p != '\0'  &&  c < 100)
        {
          if (*p == '"')
            ind = !ind;

          if ((*p == ';'  || *p == '\n') && !ind)
          {
            *p = '\0';
            p++;
            field[c] = p;
            c++;
          }
          else
          {
            p++;
          }
        }

        if (c >= 2 && field[0][0] != '\0' &&  field[1][0] != '\0')
        {
          mProducerList.push_back(std::pair<std::string,T::GeometryId>(std::string(field[0]),atoi(field[1])));
        }
      }
    }
    fclose(file);

    mProducerFileModificationTime = getFileModificationTime(mProducerFile.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::getGeometryIdListByCoordinates(QueryCoordinates& coordinates,std::set<T::GeometryId>& geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::map<T::GeometryId,uint> countList;
    uint maxCount = 0;

    for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        std::set<T::GeometryId> idList;
        Identification::gridDef.getGeometryIdListByLatLon(coordinate->y(),coordinate->x(),idList);

        for (auto g = idList.begin(); g != idList.end(); ++g)
        {
          auto c = countList.find(*g);
          if (c != countList.end())
          {
            c->second++;
            if (c->second > maxCount)
              maxCount = c->second;
          }
          else
          {
            if (maxCount == 0)
              maxCount = 1;
            countList.insert(std::pair<T::GeometryId,uint>(*g,1));
          }
        }
      }
    }

    for (auto it = countList.begin(); it != countList.end(); ++it)
    {
      if (it->second == maxCount)
      {
        geometryIdList.insert(it->first);
        //printf("Supported geometry %u\n",it->first);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ServiceImplementation::getGridValues(
                        Producer_vec& producers,
                        uint producerId,
                        uint generationFlags,
                        std::string parameterKey,
                        T::ParamLevelId paramLevelId,
                        T::ParamLevel paramLevel,
                        T::ForecastType forecastType,
                        T::ForecastNumber forecastNumber,
                        T::AreaInterpolationMethod interpolationMethod,
                        std::string forecastTime,
                        std::string analysisTime,
                        bool timeMatchRequired,
                        QueryCoordinates& coordinates,
                        std::set<T::GeometryId>& geometryIdList,
                        bool areaSearch,
                        bool reverseGenerations,
                        double radius,
                        ParameterValues& valueList)
{
  FUNCTION_TRACE
  try
  {
    std::string startTime = forecastTime;
    std::string endTime = forecastTime;

    T::AreaInterpolationMethod interpolation = interpolationMethod;

    PRINT_DATA(mDebugLog,"* getGridValues(%s:%d:%d:%d:%d:%u:%u,%s,%u)\n",
        parameterKey.c_str(),(int)paramLevelId,(int)paramLevel,(int)forecastType,(int)forecastNumber,
        producerId,generationFlags,forecastTime.c_str(),(uint)areaSearch);

    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch)
    {
      for (size_t t=0; t<coordinates.size(); t++)
      {
        if (coordinates[t].size() < 2)
        {
          SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 2 coordinates!");
          throw exception;
        }
      }
    }

    uint flags = 0;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog,"  - Producer and Geometry : %s:%d\n",producerName.c_str(),producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,producerName,producerInfo) == 0 &&  (producerId == 0  ||  producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog,"  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          uint gLen = generationInfoList.getLength();
          if (gLen == 0)
            PRINT_DATA(mDebugLog,"    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimes;
          generationInfoList.getAnalysisTimes(analysisTimes,!reverseGenerations);

          // Going through all the parameter mapping files, until we find a match.

          for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            m->getMappings(producerInfo.mName,parameterKey,true,mappings);

            if (mappings.size() == 0)
              PRINT_DATA(mDebugLog,"    - No parameter mappings found (%s:%s)!\n",producerInfo.mName.c_str(),parameterKey.c_str());

            if (mappings.size() > 0)
            {
              uint gCount = 0;

              PRINT_DATA(mDebugLog,"  - Going through the generations from the newest to the oldest.\n");

              for (auto gTime = analysisTimes.begin(); gTime != analysisTimes.end(); ++gTime)
              {
                PRINT_DATA(mDebugLog,"    * %s\n",gTime->c_str());

                uint gflags = 1 << gCount;

                bool generationValid = false;

                if (generationFlags == 0 || (generationFlags & gflags) != 0)
                  generationValid = true;

                if (analysisTime.length() > 0  &&  analysisTime != *gTime)
                  generationValid = false;

                if (generationValid)
                {
                  T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByAnalysisTime(*gTime);

                  PRINT_DATA(mDebugLog,"      - Going through the parameter mappings\n");
                  for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                  {
                    if (paramLevelId > 0 || paramLevel != 0)
                    {
                      pInfo->mParameterLevelId = paramLevelId;
                      pInfo->mParameterLevel = paramLevel;
                    }

                    if (interpolationMethod == T::AreaInterpolationMethod::Undefined)
                      interpolation = pInfo->mAreaInterpolationMethod;

                    PRINT_DATA(mDebugLog,"         + %s:%d:%d:%d:%d\n",pInfo->mParameterKey.c_str(),
                        (int)pInfo->mParameterLevelId,(int)pInfo->mParameterLevel,
                        (int)forecastType,(int)forecastNumber);

                    T::ContentInfoList contentList;
                    int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,pInfo->mParameterLevelIdType,pInfo->mParameterLevelId,pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,forecastTime,contentList);
                    if (result != 0)
                    {
                      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                      exception.addParameter("Service","getContentListByParameterGenerationIdAndForecastTime");
                      exception.addParameter("Message",ContentServer::getResultString(result));
                      throw exception;
                    }

                    PRINT_DATA(mDebugLog,"         + Found %u content records\n",contentList.getLength());

                    bool multipleOptions = contentList.containsSameForecastTimes();
                    if (multipleOptions)
                    {
                      PRINT_DATA(mDebugLog,"         + Content records contains multiple values with the same timestep\n");
                      if (mDebugLog != NULL)
                      {
                        std::stringstream stream;
                        contentList.print(stream,0,4);
                        PRINT_DATA(mDebugLog,"%s",stream.str().c_str());
                      }
                    }

                    uint contentLen = contentList.getLength();

                    if (contentLen > 0)
                    {
                      // We found content information close to the current forecast time

                      valueList.mParameterKeyType = pInfo->mParameterKeyType;
                      valueList.mParameterKey = pInfo->mParameterKey;
                      valueList.mParameterLevelIdType = pInfo->mParameterLevelIdType;
                      valueList.mParameterLevelId = pInfo->mParameterLevelId;

                      if (contentLen == 1)
                      {
                        T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(0);
                        if (contentInfo->mForecastTime == forecastTime)
                        {
                          // We found a grid which forecast time is exactly the same as the requested forecast time.

                          valueList.mForecastTime = forecastTime;
                          valueList.mProducerId = contentInfo->mProducerId;
                          valueList.mGenerationId = contentInfo->mGenerationId;
                          valueList.mGeometryId = contentInfo->mGeometryId;
                          valueList.mForecastType = contentInfo->mForecastType;
                          valueList.mForecastNumber = contentInfo->mForecastNumber;
                          valueList.mParameterLevel = contentInfo->mParameterLevel;
                          if (pInfo->mParameterLevelIdType == T::ParamLevelIdType::FMI)
                            valueList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
                          else
                            valueList.mParameterLevelId = pInfo->mParameterLevelId;

                          if (!areaSearch)
                          {
                            if (!multipleOptions)
                            {
                              // pInfo->print(std::cout,0,0);
                              if (interpolationMethod >= T::AreaInterpolationMethod::External  &&  interpolationMethod != T::AreaInterpolationMethod::Undefined  &&  coordinates[0].size() == 1)
                              {
                                double_vec valueVector;
                                double x = coordinates[0][0].x();
                                double y = coordinates[0][0].y();
                                int result = mDataServerPtr->getGridValueVectorByPoint(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,x,y,(uint)interpolationMethod,valueVector);
                                if (result != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueVectorByPoint");
                                  exception.addParameter("Message",DataServer::getResultString(result));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }

                                char tmp[1000];
                                tmp[0] = '\0';
                                char *p = tmp;
                                for (auto vv = valueVector.begin(); vv != valueVector.end(); ++vv)
                                   p += sprintf(p,"%f;",*vv);

                                valueList.mValueList.addGridValue(new T::GridValue(x,y,std::string(tmp)));
                              }
                              else
                              {
                                //pInfo->print(std::cout,0,0);
                                if (radius > 0  &&  coordinates[0].size() == 1)
                                {
                                  int result = mDataServerPtr->getGridValueListByCircle(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),radius,valueList.mValueList);
                                  if (result != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByCircle");
                                    exception.addParameter("Message",DataServer::getResultString(result));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }
                                }
                                else
                                {
                                  int result = mDataServerPtr->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0],interpolation,valueList.mValueList);
                                  if (result != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByPointList");
                                    exception.addParameter("Message",DataServer::getResultString(result));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }
                                }
                              }
                            }
                          }
                          else
                          {
                            if (!multipleOptions)
                            {
                              if (coordinates[0].size() == 2)
                              {
                                // This is a bounding box
                                int result = mDataServerPtr->getGridValueListByRectangle(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),coordinates[0][1].x(),coordinates[0][1].y(),valueList.mValueList);
                                if (result != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByRectangle");
                                  exception.addParameter("Message",DataServer::getResultString(result));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }
                              }
                              else
                              {
                                // This is a polygn
                                int result = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates,valueList.mValueList);
                                if (result != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByPolygonPath");
                                  exception.addParameter("Message",DataServer::getResultString(result));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }
                              }
                            }
                          }
                          return;
                        }
                        else
                        {
                          // There is one content record in place, but its time does not match to
                          // the requested forecast time. This is used for indicating that there
                          // are content records available, but not for the requested time.
                          // So, we should use this producer.

                          producerId = producerInfo.mProducerId;
                          valueList.mProducerId = producerId;
                        }
                      }

                      if (contentLen == 2 &&  !timeMatchRequired)
                      {
                        T::ContentInfo *contentInfo1 = contentList.getContentInfoByIndex(0);
                        T::ContentInfo *contentInfo2 = contentList.getContentInfoByIndex(1);

                        boost::posix_time::ptime forecastTime1 = toTimeStamp(contentInfo1->mForecastTime);
                        boost::posix_time::ptime forecastTime2 = toTimeStamp(contentInfo2->mForecastTime);

                        boost::posix_time::ptime forecastTimeParam = toTimeStamp(forecastTime);


                        if (contentInfo1->mForecastTime < forecastTime  &&  contentInfo2->mForecastTime > forecastTime)
                        {
                          valueList.mForecastTime = forecastTime;
                          valueList.mProducerId = contentInfo1->mProducerId;
                          valueList.mGenerationId = contentInfo1->mGenerationId;
                          valueList.mGeometryId = contentInfo1->mGeometryId;
                          valueList.mForecastType = contentInfo1->mForecastType;
                          valueList.mForecastNumber = contentInfo1->mForecastNumber;
                          valueList.mParameterLevel = contentInfo1->mParameterLevel;
                          if (pInfo->mParameterLevelIdType == T::ParamLevelIdType::FMI)
                            valueList.mParameterLevelId = contentInfo1->mFmiParameterLevelId;
                          else
                            valueList.mParameterLevelId = pInfo->mParameterLevelId;

                          T::GridValueList list1;
                          T::GridValueList list2;

                          time_t diff = toTimeT(forecastTimeParam) - toTimeT(forecastTime1);
                          time_t ttDiff = toTimeT(forecastTime2) - toTimeT(forecastTime1);

                          int result1 = 0;
                          int result2 = 0;
                          if (!areaSearch)
                          {
                            if (!multipleOptions)
                            {
                              if (interpolationMethod >= T::AreaInterpolationMethod::External  &&  interpolationMethod != T::AreaInterpolationMethod::Undefined  &&  coordinates[0].size() == 1)
                              {
                                double_vec valueVector1;
                                double_vec valueVector2;
                                double x = coordinates[0][0].x();
                                double y = coordinates[0][0].y();

                                int result1 = mDataServerPtr->getGridValueVectorByPoint(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,x,y,(uint)interpolationMethod,valueVector1);
                                if (result1 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueBlockByPoint");
                                  exception.addParameter("Message",DataServer::getResultString(result1));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }

                                int result2 = mDataServerPtr->getGridValueVectorByPoint(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,x,y,(uint)interpolationMethod,valueVector2);
                                if (result2 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueVectorByPoint");
                                  exception.addParameter("Message",DataServer::getResultString(result2));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }

                                char tmp[1000];
                                tmp[0] = '\0';
                                char *p = tmp;

                                uint vLen1 = (uint)valueVector1.size();
                                uint vLen2 = (uint)valueVector2.size();

                                if (vLen1 == vLen2  &&  vLen1 > 1)
                                {
                                  // The first number indicates the number of the constant values
                                  uint sLen = (uint)valueVector1[0];

                                  // Copying constant values:
                                  for (uint s=0; s<=sLen; s++)
                                    p += sprintf(p,"%f;",valueVector1[s]);

                                  // Copying values that can be interpolated
                                  for (uint vv=sLen+1; vv < vLen1; vv++)
                                  {
                                    T::ParamValue valueDiff = valueVector2[vv] - valueVector1[vv];
                                    T::ParamValue valueStep = valueDiff / (T::ParamValue)ttDiff;

                                    T::ParamValue newValue = valueVector1[vv] + (T::ParamValue)(diff * valueStep);
                                    p += sprintf(p,"%f;",newValue);
                                  }
                                }

                                valueList.mValueList.addGridValue(new T::GridValue(x,y,std::string(tmp)));
                              }
                              else
                              {
                                if (radius > 0  &&  coordinates[0].size() == 1)
                                {
                                  result1 = mDataServerPtr->getGridValueListByCircle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),radius,list1);
                                  if (result1 != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByCircle");
                                    exception.addParameter("Message",DataServer::getResultString(result1));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }

                                  result2 = mDataServerPtr->getGridValueListByCircle(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),radius,list2);
                                  if (result2 != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByCircle");
                                    exception.addParameter("Message",DataServer::getResultString(result2));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }
                                }
                                else
                                {
                                  result1 = mDataServerPtr->getGridValueListByPointList(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0],interpolation,list1);
                                  if (result1 != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByPointList");
                                    exception.addParameter("Message",DataServer::getResultString(result1));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }

                                  result2 = mDataServerPtr->getGridValueListByPointList(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0],interpolation,list2);
                                  if (result2 != 0)
                                  {
                                    SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                    exception.addParameter("Service","getGridValueListByPointList");
                                    exception.addParameter("Message",DataServer::getResultString(result2));
                                    std::string errorMsg = exception.getStackTrace();
                                    PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                    //throw exception;
                                  }
                                }
                              }
                            }
                          }
                          else
                          {
                            if (!multipleOptions)
                            {
                              if (coordinates[0].size() == 2)
                              {
                                // This is a bounding box
                                result1 = mDataServerPtr->getGridValueListByRectangle(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),coordinates[0][1].x(),coordinates[0][1].y(),list1);
                                if (result1 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByRectangle");
                                  exception.addParameter("Message",DataServer::getResultString(result1));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }

                                result2 = mDataServerPtr->getGridValueListByRectangle(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),coordinates[0][1].x(),coordinates[0][1].y(),list2);
                                if (result2 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByRectangle");
                                  exception.addParameter("Message",DataServer::getResultString(result2));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }
                              }
                              else
                              {
                                result1 = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates,list1);
                                if (result1 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByPointList");
                                  exception.addParameter("Message",DataServer::getResultString(result1));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }

                                result2 = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates,list2);
                                if (result2 != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByPolygonPath");
                                  exception.addParameter("Message",DataServer::getResultString(result2));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }
                              }
                            }
                          }

                          uint len1 = list1.getLength();
                          uint len2 = list2.getLength();

                          if (result1 == 0  &&  result2 == 0  &&  len1 == len2)
                          {
                            for (uint t=0; t<len1; t++)
                            {
                              T::GridValue *val1 = list1.getGridValueByIndex(t);
                              T::GridValue *val2 = list2.getGridValueByIndex(t);

                              T::ParamValue valueDiff = val2->mValue - val1->mValue;
                              T::ParamValue valueStep = valueDiff / (T::ParamValue)ttDiff;

                              T::GridValue *rec = new T::GridValue();
                              rec->mX = val1->mX;
                              rec->mY = val1->mY;
                              rec->mValue = val1->mValue + (T::ParamValue)(diff * valueStep);
                              valueList.mValueList.addGridValue(rec);
                            }
                          }
                          return;
                        }
                        else
                        {
                          SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
                          exception.addDetail("The given forecast time should been between the found content times.");
                          exception.addParameter("Content 1 ForecastTime",contentInfo1->mForecastTime);
                          exception.addParameter("Content 2 ForecastTime",contentInfo2->mForecastTime);
                          exception.addParameter("Request ForecastTime",forecastTime);
                          throw exception;
                        }
                      }
                    }
                    else
                    {
                      // No content found.

                      T::ContentInfoList contentInfoList;
                      mContentServerPtr->getContentListByParameterAndProducerId(0,producerInfo.mProducerId,pInfo->mParameterKeyType,pInfo->mParameterKey,pInfo->mParameterLevelIdType,pInfo->mParameterLevelId,pInfo->mParameterLevel,pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,startTime,endTime,0,contentInfoList);
                    }
                  }
                }
                gCount++;
              }
            }
          }
        }
        else
        {
          PRINT_DATA(mDebugLog,"  - Not a valid producer '%s'!\n",producerName.c_str());
        }
      }
      else
      {
        PRINT_DATA(mDebugLog,"  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n",producerName.c_str(),producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getGridValues(
                        Producer_vec& producers,
                        uint producerId,
                        uint generationFlags,
                        std::string parameterKey,
                        T::ParamLevelId paramLevelId,
                        T::ParamLevel paramLevel,
                        T::ForecastType forecastType,
                        T::ForecastNumber forecastNumber,
                        T::AreaInterpolationMethod interpolationMethod,
                        std::string startTime,
                        std::string endTime,
                        std::string analysisTime,
                        QueryCoordinates& coordinates,
                        std::set<T::GeometryId>& geometryIdList,
                        bool ignoreStartTimeValue,
                        bool areaSearch,
                        bool reverseGenerations,
                        double radius,
                        uint maxValues,
                        ParameterValues_vec& valueList)
{
  FUNCTION_TRACE
  try
  {
    PRINT_DATA(mDebugLog,"* getGridValues(%s:%d:%d:%d:%d:%u:%u,%s,%s,%u)\n",
        parameterKey.c_str(),(int)paramLevelId,(int)paramLevel,(int)forecastType,(int)forecastNumber,
        producerId,generationFlags,startTime.c_str(),endTime.c_str(),(uint)areaSearch);

    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch)
    {
      for (size_t t=0; t<coordinates.size(); t++)
      {
        if (coordinates[t].size() < 2)
        {
          SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 2 coordinates!");
          throw exception;
        }
      }
    }

    T::AreaInterpolationMethod interpolation = interpolationMethod;

    std::string minStartTime = "30000101T000000";
    std::string maxEndTime = "15000101T000000";

    uint flags = 0;

    // Going through the producer list.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;
      T::GeometryId producerGeometryId = it->second;

      PRINT_DATA(mDebugLog,"  - Producer and Geometry : %s:%d\n",producerName.c_str(),producerGeometryId);

      // Checking if the current geometry can be found from the acceptable geometry list

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.

        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,producerName,producerInfo) == 0  &&  (producerId == 0  ||  producerInfo.mProducerId == producerId))
        {
          PRINT_DATA(mDebugLog,"  - The producer and the geometry are acceptable!\n");

          // Reading generations supported by the current producer.

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          uint gLen = generationInfoList.getLength();

          if (gLen == 0)
            PRINT_DATA(mDebugLog,"    - No generations found for the current producer!\n");

          // Sorting generation analysis times.

          std::vector<std::string> analysisTimes;
          generationInfoList.getAnalysisTimes(analysisTimes,!reverseGenerations);

          // Going through all the parameter mapping files, until we find a match.

          for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            m->getMappings(producerInfo.mName,parameterKey,true,mappings);

            if (mappings.size() == 0)
              PRINT_DATA(mDebugLog,"    - No parameter mappings found (%s:%s)!\n",producerInfo.mName.c_str(),parameterKey.c_str());

            if (mappings.size() > 0)
            {
              uint gCount = 0;

              if (reverseGenerations)
              {
                PRINT_DATA(mDebugLog,"    - Going through the generations from the oldest to the newest.\n");
              }
              else
              {
                PRINT_DATA(mDebugLog,"    - Going through the generations from the newest to the oldest.\n");
              }

              for (auto gTime = analysisTimes.begin(); gTime != analysisTimes.end(); ++gTime)
              {
                PRINT_DATA(mDebugLog,"    * %s\n",gTime->c_str());

                uint gflags = 1 << gCount;

                bool generationValid = false;

                if (generationFlags == 0 || (generationFlags & gflags) != 0)
                  generationValid = true;

                if (analysisTime.length() > 0  &&  analysisTime != *gTime)
                  generationValid = false;

                if (generationValid)
                {
                  T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByAnalysisTime(*gTime);

                  PRINT_DATA(mDebugLog,"      - Going through the parameter mappings\n");
                  for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                  {
                    if (paramLevelId > 0 || paramLevel != 0)
                    {
                      pInfo->mParameterLevelId = paramLevelId;
                      pInfo->mParameterLevel = paramLevel;
                    }

                    if (interpolationMethod == T::AreaInterpolationMethod::Undefined)
                      interpolation = pInfo->mAreaInterpolationMethod;

                    PRINT_DATA(mDebugLog,"         + %s:%d:%d:%d:%d\n",pInfo->mParameterKey.c_str(),
                        (int)pInfo->mParameterLevelId,(int)pInfo->mParameterLevel,
                        (int)forecastType,(int)forecastNumber);

                    T::ContentInfoList contentList;
                    uint requestFlags = (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER;

                    int result = mContentServerPtr->getContentListByParameterAndGenerationId(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,pInfo->mParameterLevelIdType,pInfo->mParameterLevelId,pInfo->mParameterLevel,pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,startTime,endTime,requestFlags,contentList);
                    if (result != 0)
                    {
                      SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                      exception.addParameter("Service","getContentListByParameterAndGenerationId");
                      exception.addParameter("Message",ContentServer::getResultString(result));
                      throw exception;
                    }

                    PRINT_DATA(mDebugLog,"         + Found %u content records\n",contentList.getLength());

                    bool multipleOptions = contentList.containsSameForecastTimes();
                    if (multipleOptions)
                    {
                      PRINT_DATA(mDebugLog,"         + Content records contains multiple values with the same timestep\n");
                      if (mDebugLog != NULL)
                      {
                        std::stringstream stream;
                        contentList.print(stream,0,4);
                        PRINT_DATA(mDebugLog,"%s",stream.str().c_str());
                      }
                    }

                    //std::string lastTime = startTime;

                    uint clen = contentList.getLength();

                    T::ContentInfo *firstContentInfo = contentList.getContentInfoByIndex(0);

                    for (uint t=0; t<clen; t++)
                    {
                      T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(t);

                      // Let's use this producer for now on for this parameter.
                      producerId = contentInfo->mProducerId;

                      // std::cout << contentInfo->mForecastTime << " < " << minStartTime  << " STARTTIME\n";
                      // std::cout << contentInfo->mForecastTime << " > " << maxEndTime  << " ENDTIME\n";


                      if (((contentInfo->mForecastTime == startTime  &&  !ignoreStartTimeValue) || (contentInfo->mForecastTime > startTime  &&  contentInfo->mForecastTime <= endTime))  &&  (contentInfo->mForecastTime < minStartTime  ||  contentInfo->mForecastTime > maxEndTime))
                      {
                        ParameterValues valList;
                        valList.mForecastTime = contentInfo->mForecastTime;
                        valList.mProducerId = contentInfo->mProducerId;
                        valList.mGenerationId = contentInfo->mGenerationId;
                        valList.mGeometryId = contentInfo->mGeometryId;
                        valList.mParameterKeyType = pInfo->mParameterKeyType;
                        valList.mParameterKey = pInfo->mParameterKey;
                        valList.mParameterLevelIdType = pInfo->mParameterLevelIdType;
                        if (pInfo->mParameterLevelIdType == T::ParamLevelIdType::FMI)
                          valList.mParameterLevelId = contentInfo->mFmiParameterLevelId;
                        else
                          valList.mParameterLevelId = pInfo->mParameterLevelId;
                        valList.mParameterLevel = contentInfo->mParameterLevel;
                        valList.mForecastType = contentInfo->mForecastType;
                        valList.mForecastNumber = contentInfo->mForecastNumber;

                        int result = 0;
                        if (!areaSearch)
                        {
                          if (!multipleOptions)
                          {
                            if (interpolationMethod >= T::AreaInterpolationMethod::External  &&  interpolationMethod != T::AreaInterpolationMethod::Undefined  &&  coordinates[0].size() == 1)
                            {
                              double_vec valueVector;
                              double x = coordinates[0][0].x();
                              double y = coordinates[0][0].y();
                              int result = mDataServerPtr->getGridValueVectorByPoint(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,x,y,(uint)interpolationMethod,valueVector);
                              if (result != 0)
                              {
                                SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                exception.addParameter("Service","getGridValueBlockByPoint");
                                exception.addParameter("Message",DataServer::getResultString(result));
                                std::string errorMsg = exception.getStackTrace();
                                PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                //throw exception;
                              }

                              char tmp[1000];
                              tmp[0] = '\0';
                              char *p = tmp;
                              for (auto vv = valueVector.begin(); vv != valueVector.end(); ++vv)
                                 p += sprintf(p,"%f;",*vv);

                              valList.mValueList.addGridValue(new T::GridValue(x,y,std::string(tmp)));
                            }
                            else
                            {
                              if (radius > 0  &&  coordinates[0].size() == 1)
                              {
                                int result = mDataServerPtr->getGridValueListByCircle(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),radius,valList.mValueList);
                                if (result != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByCircle");
                                  exception.addParameter("Message",DataServer::getResultString(result));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //throw exception;
                                }
                              }
                              else
                              {
                                result = mDataServerPtr->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0],interpolation,valList.mValueList);
                                if (result != 0)
                                {
                                  SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                  exception.addParameter("Service","getGridValueListByPointList");
                                  exception.addParameter("Message",DataServer::getResultString(result));
                                  std::string errorMsg = exception.getStackTrace();
                                  PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                  //exception.printError();
                                  //throw exception;
                                }
                              }
                            }
                          }
                        }
                        else
                        {
                          if (!multipleOptions)
                          {
                            if (coordinates[0].size() == 2)
                            {
                              // This is a bounding box
                              result = mDataServerPtr->getGridValueListByRectangle(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates[0][0].x(),coordinates[0][0].y(),coordinates[0][1].x(),coordinates[0][1].y(),valList.mValueList);
                              if (result != 0)
                              {
                                SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                exception.addParameter("Service","getGridValueListByRectangle");
                                exception.addParameter("Message",DataServer::getResultString(result));
                                std::string errorMsg = exception.getStackTrace();
                                PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                //exception.printError();
                                //throw exception;
                              }
                            }
                            else
                            {
                              result = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,flags,T::CoordinateType::LATLON_COORDINATES,coordinates,valList.mValueList);
                              if (result != 0)
                              {
                                SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                                exception.addParameter("Service","getGridValueListByPolygonPath");
                                exception.addParameter("Message",DataServer::getResultString(result));
                                std::string errorMsg = exception.getStackTrace();
                                PRINT_DATA(mDebugLog,"%s\n",errorMsg.c_str());
                                //exception.printError();
                                //throw exception;
                              }
                            }
                          }
                        }

                        if (result == 0)
                        {
                          if (contentInfo->mForecastTime < minStartTime)
                          {
                            minStartTime = contentInfo->mForecastTime;
                            valueList.insert(valueList.begin(),valList);
                          }

                          if (contentInfo->mForecastTime > maxEndTime  &&  minStartTime != contentInfo->mForecastTime)
                          {
                            maxEndTime = contentInfo->mForecastTime;
                            valueList.push_back(valList);
                          }

                          if (valueList.size() == maxValues)
                            return;
                        }
                      }

                      if (contentInfo->mForecastTime >= endTime  &&  firstContentInfo->mForecastTime <= startTime)
                      {
                        // We have found the full time range from this generation, so we
                        // should end the search. It is good to notice that the forecast
                        // time of the last content is not necessary the same as the exact
                        // end time.

                        if (!multipleOptions)
                          return;
                      }
                    }

                    if (minStartTime <= startTime  &&  maxEndTime >= endTime)
                    {
                      if (!multipleOptions)
                        return;
                    }
                  }
                }
                else
                {
                  PRINT_DATA(mDebugLog,"      - Generation not included into the search\n");
                }
                gCount++;
              }
            }
          }
        }
        else
        {
          PRINT_DATA(mDebugLog,"  - Not a valid producer '%s'!\n",producerName.c_str());
        }
      }
      else
      {
        PRINT_DATA(mDebugLog,"  - Producer's '%s' geometry '%d' does not cover all requested coordinates!\n",producerName.c_str(),producerGeometryId);
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::getParameterFunctionInfo(std::string paramStr,std::string& function,std::string& functionParams)
{
  FUNCTION_TRACE
  try
  {
    char buf[1000];
    char tmp[1000];

    uint c = 0;
    strcpy(buf,paramStr.c_str());
    char *p = buf;

    while (*p != '\0'  &&  *p != '{')
    {
      p++;
    }

    if (*p == '{')
    {
      *p = '\0';
      p++;
      int level = 1;
      while (*p != '\0')
      {
        if (*p == '}')
        {
          level--;
          if (level == 0)
          {
            tmp[c] = '\0';
            function = buf;
            functionParams = tmp;
            //printf("FUNCTION [%s] [%s]\n",buf,tmp);
            return true;
          }
        }
        else
        if (*p == '{')
          level++;

        tmp[c] = *p;
        c++;
        p++;
      }
    }
    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::getFunctionParams(std::string functionParamsStr,FunctionParam_vec& functionParams)
{
  FUNCTION_TRACE
  try
  {
    bool containsFunction = false;
    char buf[1000];
    char tmp[1000];
    uint c = 0;
    strcpy(buf,functionParamsStr.c_str());

    char *p = buf;
    uint level = 0;
    while (*p != '\0')
    {
      if (*p == '{')
      {
        containsFunction = true;
        level++;
      }

      if (*p == '}')
        level--;

      if (*p == ';'  &&  level == 0)
      {
        tmp[c] = '\0';
        //printf("PARAM [%s]\n",tmp);
        mFunctionParamId++;
        functionParams.push_back(std::pair<uint,std::string>(mFunctionParamId,std::string(tmp)));
        c = 0;
      }
      else
      {
        tmp[c] = *p;
        c++;
      }
      p++;
    }

    if (c > 0)
    {
      tmp[c] = '\0';
      //printf("PARAM [%s]\n",tmp);
      mFunctionParamId++;
      functionParams.push_back(std::pair<uint,std::string>(mFunctionParamId,std::string(tmp)));
    }
    return containsFunction;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getParameterStringInfo(std::string param,std::string& key,T::ParamLevelId& paramLevelId,T::ParamLevel& paramLevel,T::ForecastType& forecastType,T::ForecastNumber& forecastNumber,uint& producerId,uint& generationFlags,T::AreaInterpolationMethod& interpolationMethod)
{
  FUNCTION_TRACE
  try
  {
    char buf[1000];
    strcpy(buf,param.c_str());

    char *field[100];
    uint c = 1;
    field[0] = buf;
    char *p = buf;
    int level = 0;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == '{')
        level++;

      if (*p == '}')
        level--;

      if (level == 0  &&  *p == ':')
      {
        *p = '\0';
        p++;
        field[c] = p;
        c++;
      }
      else
      {
        p++;
      }
    }

    //for (uint t=0; t<c; t++)
    //  printf("FIELD (%s) %u [%s]\n",buf,t,field[t]);


    key = field[0];

    if (c > 1)
    {
      if (field[1][0] != '\0')
      {
        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,std::string(field[1]),producerInfo) == 0)
          producerId = producerInfo.mProducerId;
      }
    }

    if (c > 2)
    {
      if (field[2][0] != '\0')
        paramLevelId = (T::ParamLevelId)atoi(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0')
        paramLevel = atoi(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0')
        forecastType = (T::ForecastType)atoi(field[4]);
    }

    if (c > 5)
    {
      if (field[5][0] != '\0')
        forecastNumber = (T::ForecastNumber)atoi(field[5]);
    }

    if (c > 6)
    {
      if (field[6][0] != '\0')
        generationFlags = atoi(field[6]);
    }

    if (c > 7)
    {
      if (field[7][0] != '\0')
        interpolationMethod = (T::AreaInterpolationMethod)atoi(field[7]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::getAlias(std::string name,std::string& alias)
{
  FUNCTION_TRACE
  try
  {
    return mAliasFileCollection.getAlias(name,alias);
    /*
    for (auto it = mAliases.begin(); it != mAliases.end(); ++it)
    {
      if (it->getAlias(name,alias))
        return true;
    }
    return false;
    */
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::parseFunction(QueryParameter& queryParam,std::string paramStr,std::string& function,FunctionParam_vec& functionParams,uint recursionCounter,std::vector<QueryParameter>& additionalParameterList)
{
  FUNCTION_TRACE
  try
  {
    if (recursionCounter > 20)
    {
      SmartMet::Spine::Exception exception(BCP, "The recursion is too deep!");
      exception.addDetail("It seems that the function parsing is in a continuous loop.");
      exception.addDetail("The reason for this is most likely that some aliases refer to each other.");
      exception.addParameter("ParamStr",paramStr);
      throw exception;
    }

    PRINT_DATA(mDebugLog,"   + Parse parameter : %s\n",paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr,function,functionParamsStr))
    {
      PRINT_DATA(mDebugLog,"     -- Function: %s\n",function.c_str());
      PRINT_DATA(mDebugLog,"     -- Params  : %s\n",functionParamsStr.c_str());

      getFunctionParams(functionParamsStr,functionParams);

      for (auto fParam = functionParams.begin(); fParam != functionParams.end(); ++fParam)
      {
        if (atof(fParam->second.c_str()) == 0)
        {
          QueryParameter newParam;
          newParam.mId = fParam->first;
          newParam.mParam = fParam->second;
          newParam.mOrigParam = paramStr + " => " + fParam->second;
          newParam.mSymbolicName = fParam->second;
          newParam.mParameterLevelId = queryParam.mParameterLevelId;
          newParam.mParameterLevel = queryParam.mParameterLevel;
          newParam.mForecastType = queryParam.mForecastType;
          newParam.mForecastNumber = queryParam.mForecastNumber;
          newParam.mAreaInterpolationMethod = queryParam.mAreaInterpolationMethod;
          newParam.mProducerId = queryParam.mProducerId;
          newParam.mGenerationFlags = queryParam.mGenerationFlags;
          newParam.mTemporary = true;
          newParam.mAdditionalTimeList = queryParam.mAdditionalTimeList;

          std::string alias;
          if (getAlias(fParam->second,alias))
          {
            newParam.mParam = alias;
            newParam.mOrigParam = newParam.mOrigParam + " => " + alias;
            std::string paramName;
            getParameterStringInfo(fParam->second,paramName,newParam.mParameterLevelId,newParam.mParameterLevel,newParam.mForecastType,newParam.mForecastNumber,newParam.mProducerId,newParam.mGenerationFlags,newParam.mAreaInterpolationMethod);

          }
          //newParam.print(std::cout,0,0);

          parseFunction(newParam,newParam.mParam,newParam.mFunction,newParam.mFunctionParams,recursionCounter+1,additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(),newParam);
        }
      }
      return true;
    }
    else
    {
      std::string paramName;
      getParameterStringInfo(paramStr,paramName,queryParam.mParameterLevelId,queryParam.mParameterLevel,queryParam.mForecastType,queryParam.mForecastNumber,queryParam.mProducerId,queryParam.mGenerationFlags,queryParam.mAreaInterpolationMethod);

      std::string alias;
      if (getAlias(paramName,alias))
      {
        if (alias != paramStr)
        {
          if (!parseFunction(queryParam,alias,function,functionParams,recursionCounter+1,additionalParameterList))
            queryParam.mParam = alias;
        }
      }
      return false;
    }
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP, "Parsing failed!", NULL);
    exception.addParameter("ParamStr",paramStr);
    throw exception;
  }
}




#if 0
std::string ServiceImplementation::executeFunctionCall5(std::string& function,std::string language,std::vector<double>& parameters)
{
  FUNCTION_TRACE
  try
  {
    return mLuaFileCollection.executeFunctionCall5(function,language,parameters);
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP, "Operation failed!", NULL);
    exception.addParameter("Function",function);
    throw exception;
  }
}





double ServiceImplementation::executeFunctionCall1(std::string& function,std::vector<double>& parameters)
{
  FUNCTION_TRACE
  try
  {
    return mLuaFileCollection.executeFunctionCall1(function,parameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}
#endif




void ServiceImplementation::checkConfigurationUpdates()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if ((time(0) - mLastConfiguratonCheck) > mCheckInterval)
    {
      mLastConfiguratonCheck = time(0);

      time_t t1 = getFileModificationTime(mProducerFile.c_str());

      if (mProducerFileModificationTime != t1  &&  (mLastConfiguratonCheck - t1) > 3)
        loadProducerFile();

      for (auto it = mParameterMappings.begin(); it != mParameterMappings.end(); ++it)
      {
        it->checkUpdates();
      }
    }

    mAliasFileCollection.checkUpdates(false);
    mLuaFileCollection.checkUpdates(false);
    mProducerAliasFile.checkUpdates();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getProducerList(Query& query,Producer_vec& producers)
{
  FUNCTION_TRACE
  try
  {
    if (query.mProducerNameList.size() == 0)
    {
      producers = mProducerList;
      return;
    }

    for (auto pName = query.mProducerNameList.begin(); pName != query.mProducerNameList.end(); ++pName)
    {
      for (auto it = mProducerList.begin(); it != mProducerList.end(); ++it)
      {
        if (strcasecmp(pName->c_str(),it->first.c_str()) == 0)
        {
          producers.push_back(std::pair<std::string,T::GeometryId>(it->first,it->second));
        }
        else
        {
          std::string alias;
          if (mProducerAliasFile.getAlias(*pName,alias))
          {
            if (strcasecmp(alias.c_str(),it->first.c_str()) == 0)
            {
              producers.push_back(std::pair<std::string,T::GeometryId>(it->first,it->second));
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::updateQueryParameters(Query& query)
{
  FUNCTION_TRACE
  try
  {
    bool updateRequired = true;
    uint updateCounter = 0;

    while (updateRequired  &&  updateCounter < 10)
    {
      updateRequired = false;
      updateCounter++;

      PRINT_DATA(mDebugLog,"UPDATE QUERY PARAMETERS\n");

      std::vector<QueryParameter> additionalParameterList;

      // Going through all the query parameters.

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        // The parameter can contain some level information even if the parameter is an alias
        // or a function. In this case this level information should be inherited to the alias
        // definition or to the function parameters.

        PRINT_DATA(mDebugLog," * PARAMETER: %s\n",qParam->mSymbolicName.c_str());

        getParameterStringInfo(qParam->mParam,qParam->mParameterKey,qParam->mParameterLevelId,qParam->mParameterLevel,qParam->mForecastType,qParam->mForecastNumber,qParam->mProducerId,qParam->mGenerationFlags,qParam->mAreaInterpolationMethod);


        if (qParam->mParameterKeyType == T::ParamKeyType::NEWBASE_ID)
        {
          Identification::NewbaseParameterDef paramDef;
          if (Identification::gridDef.getNewbaseParameterDefById(qParam->mParameterKey,paramDef))
          {
            // std::cout << qParam->mSymbolicName << "  => " << alias << " ( " << qParam->mParam << " )\n";
            PRINT_DATA(mDebugLog,"   + Newbase name: %s\n",paramDef.mParameterName.c_str());

            qParam->mParam = paramDef.mParameterName;
            //qParam->mParameterKeyType == T::ParamKeyType::NEWBASE_NAME;
          }
        }

        // If the parameter is a symbolic name (defined in the configuration file) then
        // we should use its definition.

        std::string alias;
        if (getAlias(qParam->mParameterKey,alias))
        {
          // std::cout << qParam->mSymbolicName << "  => " << alias << " ( " << qParam->mParam << " )\n";
          PRINT_DATA(mDebugLog,"   + Alias: %s\n",alias.c_str());

          qParam->mParam = alias;

          // When an alias is replaced by its definition, it is possible that this definition
          // contains other aliases. That's why we need to repeat this "updateQueryParameters"
          // loop. The 'updateCounter' variable is used in order to ensure that the loop
          // will end if the aliases are referencing to each other (=> eternal loop).

          updateRequired = true;
        }



        // If the parameter contains a function then we should parse it and make sure that
        // we fetch the required parameter values for it.

        if (qParam->mFunction.length() == 0)
          parseFunction(*qParam,qParam->mParam,qParam->mFunction,qParam->mFunctionParams,0,additionalParameterList);
      }

      // Adding parameters that are required by functions into the query.

      for (auto p = additionalParameterList.begin(); p != additionalParameterList.end(); ++p)
        query.mQueryParameterList.push_back(*p);

      PRINT_DATA(mDebugLog,"\n");
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::executeQueryFunctions(Query& query)
{
  FUNCTION_TRACE
  try
  {
    uint valueCount = query.getValuesPerTimeStep();

    for (auto qParam = query.mQueryParameterList.rbegin(); qParam != query.mQueryParameterList.rend(); ++qParam)
    {
      if (qParam->mFunction.length() > 0)
      {
        uint tCount = 0;
        for (auto tt = query.mForecastTimeList.begin(); tt != query.mForecastTimeList.end(); ++tt)
        {
          ParameterValues pValues;
          pValues.mForecastTime = *tt;

          std::vector<double> areaParameters;
          std::vector<double> extParameters;

          bool areaCnt = false;
          T::GridValue lastRec;
          for (uint v=0; v < valueCount; v++)
          {
            std::vector<double> parameters;

            uint producerId = 0;
            uint generationId = 0;
            uint geometryId = 0;

            for (auto it = qParam->mFunctionParams.begin(); it != qParam->mFunctionParams.end(); ++it)
            {
              double a = atof(it->second.c_str());
              QueryParameter *q = query.getQueryParameterPtr(it->first);
              std::vector<double> valueList;
              if (q != NULL)
              {
                if (tCount < (uint)q->mValueList.size())
                {
                  if (producerId == 0  &&  q->mValueList[tCount].mProducerId > 0)
                  {
                    producerId = q->mValueList[tCount].mProducerId;
                    generationId = q->mValueList[tCount].mGenerationId;
                    geometryId = q->mValueList[tCount].mGeometryId;
                  }

                  T::GridValue *rec = q->mValueList[tCount].mValueList.getGridValueByIndex(v);
                  if (rec != NULL)
                  {
                    if (/*qParam->mAreaInterpolationMethod == T::AreaInterpolationMethod::External  &&*/  rec->mValueString.length() > 0)
                    {
                      splitString(rec->mValueString,';',valueList);
                    }
                    else
                    {
                      a = rec->mValue;
                    }

                    if (rec->mX == -1000  &&  rec->mY == -1000)
                    {
                      areaCnt = true;
                    }
                    else
                    {
                      lastRec = *rec;
                    }
                  }
                  else
                  {
                    a = ParamValueMissing;
                  }
                }
                else
                {
                  areaCnt = true;
                }
              }

              if (valueList.size() == 0)
              {
                parameters.push_back(a);
                areaParameters.push_back(a);

                if (areaCnt  &&  v == 0)
                  extParameters.push_back(a);
              }
              else
              {
                for (auto aa = valueList.begin(); aa != valueList.end(); ++aa)
                {
                  parameters.push_back(*aa);
                  areaParameters.push_back(*aa);

                  if (areaCnt  &&  v == 0)
                    extParameters.push_back(*aa);
                }
              }
            }

            std::string f = qParam->mFunction.substr(0,1);

            if (f != "@")
            {
              std::string function;
              uint type = mLuaFileCollection.getFunction(qParam->mFunction,function);
              switch (type)
              {
                case 1:
                {
                  double val = mLuaFileCollection.executeFunctionCall1(function,parameters);
                  pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX,lastRec.mY,(T::ParamValue)val));
                }
                break;

                case 5:
                {
                  std::string val = mLuaFileCollection.executeFunctionCall5(function,query.mLanguage,parameters);
                  pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX,lastRec.mY,val));
                }
                break;
              }

              pValues.mProducerId = producerId;
              pValues.mGenerationId = generationId;
              pValues.mGeometryId = geometryId;
            }
          }

          std::string f = qParam->mFunction.substr(0,1);
          if (areaCnt  &&  f != "@")
          {
            //double val = executeFunctionCall1(qParam->mFunction,extParameters);
            double val = mLuaFileCollection.executeFunctionCall1(qParam->mFunction,extParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,(T::ParamValue)val));
          }

          if (qParam->mFunction.substr(0,1) == "@")
          {
            //T::ParamValue val = executeAreaFunction(qParam->mFunction,areaParameters);
            std::string func = qParam->mFunction.substr(1);
            double val = mLuaFileCollection.executeFunctionCall1(func,areaParameters);
            //double val = executeFunctionCall1(func,areaParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,(T::ParamValue)val));
          }

          if (areaCnt)
          {
            if (pValues.mValueList.getLength() > 0)
            {
              T::GridValue *rec = pValues.mValueList.getGridValueByIndex(0);
              T::ParamValue val = rec->mValue;
              pValues.mValueList.clear();
              pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,val));
            }
          }
          qParam->mValueList.push_back(pValues);

          tCount++;
        }
      }
    }

    if (mDebugLog != NULL)
    {
      std::stringstream stream;
      query.print(stream,0,0);
      PRINT_DATA(mDebugLog,"%s",stream.str().c_str());
    }

    //query.print(std::cout,0,0);

    query.removeTemporaryParameters();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    // Getting geometries that support support the given coordinates.

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(query.mCoordinateList,geometryIdList);


    std::map<std::string,uint> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool ignoreStartTimeValue = false;
    if ((query.mFlags & QF_START_TIME_NOT_INCLUDED) != 0)
      ignoreStartTimeValue = true;


    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

    bool reverseGenerations = false;
    if ((query.mFlags & QF_REVERSE_GENERATION_FLAGS) != 0)
      reverseGenerations = true;


    // Fetching parameter data according to the given time range and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      // Checking that the parameter is "a data parameter" - not a function.

      if (qParam->mFunction.length() == 0)
      {
        std::string paramName = qParam->mParam;
        T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
        T::ParamLevel paramLevel = qParam->mParameterLevel;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;
        T::AreaInterpolationMethod interpolationMethod = qParam->mAreaInterpolationMethod;
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        //qParam->print(std::cout,0,0);

        getParameterStringInfo(qParam->mParam,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,producerId,generationFlags,interpolationMethod);

        //printf("**** PARAM %s  => %d %d\n",qParam->mParam.c_str(),(int)paramLevelId,(int)paramLevel);

        if (paramName.substr(0,1) == "$")
          paramName = paramName.substr(1);

        try
        {
          if (qParam->mParameterKeyType != T::ParamKeyType::BUILD_IN)
          {
            std::string startTime = query.mStartTime;
            std::string endTime = query.mEndTime;

            if ((query.mFlags & QF_START_TIME_DATA) != 0)
            {
              startTime = "15000101T000000"; // Start time is the start time of the data
              endTime = "30000101T000000"; // End time is the end time of the data
            }

            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName);
              if (it != parameterProducers.end())
                producerId = it->second;
            }

            getGridValues(producers,producerId,generationFlags,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,interpolationMethod,startTime,endTime,query.mAnalysisTime,query.mCoordinateList,geometryIdList,ignoreStartTimeValue,areaQuery,reverseGenerations,query.mRadius,query.mMaxParameterValues,qParam->mValueList);

            if (producerId == 0  &&  qParam->mValueList.size() > 0  &&  qParam->mValueList[0].mProducerId != 0)
            {
              parameterProducers.insert(std::pair<std::string,uint>(paramName,qParam->mValueList[0].mProducerId));
            }

            if (qParam->mAdditionalTimeList.size() > 0)
            {
              for (auto fTime = qParam->mAdditionalTimeList.begin(); fTime != qParam->mAdditionalTimeList.end(); ++fTime)
              {
                ParameterValues valueList;
                if (qParam->mParameterKeyType != T::ParamKeyType::BUILD_IN)
                  getGridValues(producers,producerId,generationFlags,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,interpolationMethod,*fTime,query.mAnalysisTime,false,query.mCoordinateList,geometryIdList,areaQuery,reverseGenerations,query.mRadius,valueList);

                if (valueList.mValueList.getLength() == 0)
                  valueList.mForecastTime = *fTime;

                bool ind = false;
                for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end()  &&  !ind; ++it)
                {
                  if (it->mForecastTime > *fTime)
                  {
                    qParam->mValueList.insert(it,valueList);
                    ind = true;
                  }
                }

                if (!ind)
                  qParam->mValueList.push_back(valueList);
              }
            }
          }
        }
        catch (...)
        {
          SmartMet::Spine::Exception exception(BCP, "Operation failed!", NULL);
          exception.printError();
        }
      }
    }

    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    std::set<std::string> timeList;

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end())
          timeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the current
    // forecast time. If not, then the forecast time is added to the parameters's forecast time list, but
    // the actual value list of the current forecast time will be empty.

    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.insert(*tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() &&  !found; ++it)
          {
            if (it->mForecastTime < *tt)
              cnt++;
            else
            if (it->mForecastTime == *tt)
              found = true;
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;
            qParam->mValueList.insert(qParam->mValueList.begin() + cnt,pValues);
          }
        }
      }
    }

    // At this point we have fetched values for all data parameters. Now we are able to
    // execute functions that uses this data.

    executeQueryFunctions(query);
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::executeTimeStepQuery(Query& query)
{
  FUNCTION_TRACE
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(query.mCoordinateList,geometryIdList);

    std::map<std::string,uint> parameterProducers;

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

    bool reverseGenerations = false;
    if ((query.mFlags & QF_REVERSE_GENERATION_FLAGS) != 0)
      reverseGenerations = true;


    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    std::set<std::string> timeList;
    for (auto fTime = query.mForecastTimeList.begin(); fTime != query.mForecastTimeList.end(); ++fTime)
    {
      timeList.insert(*fTime);
    }

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      if (qParam->mFunction.length() == 0)
      {
        std::string paramName = qParam->mParam;
        T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
        T::ParamLevel paramLevel = qParam->mParameterLevel;
        T::ForecastType forecastType = qParam->mForecastType;
        T::ForecastNumber forecastNumber = qParam->mForecastNumber;
        T::AreaInterpolationMethod interpolationMethod = qParam->mAreaInterpolationMethod;
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        getParameterStringInfo(qParam->mParam,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,producerId,generationFlags,interpolationMethod);

        if (paramName.substr(0,1) == "$")
          paramName = paramName.substr(1);

        std::set<std::string> forecastTimeList = timeList;
        for (auto fTime = qParam->mAdditionalTimeList.begin(); fTime != qParam->mAdditionalTimeList.end(); ++fTime)
        {
          forecastTimeList.insert(*fTime);
        }

        for (auto fTime = forecastTimeList.begin(); fTime != forecastTimeList.end(); ++fTime)
        {
          ParameterValues valueList;
          try
          {
            if (producerId == 0)
            {
              auto it = parameterProducers.find(paramName);
              if (it != parameterProducers.end())
                producerId = it->second;
            }

            if (qParam->mParameterKeyType != T::ParamKeyType::BUILD_IN)
            {
              getGridValues(producers,producerId,generationFlags,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,interpolationMethod,*fTime,query.mAnalysisTime,false,query.mCoordinateList,geometryIdList,areaQuery,reverseGenerations,query.mRadius,valueList);

              if (producerId == 0  &&  valueList.mProducerId != 0)
              {
                parameterProducers.insert(std::pair<std::string,uint>(paramName,valueList.mProducerId));
              }
            }
          }
          catch (...)
          {
            SmartMet::Spine::Exception exception(BCP, "Operation failed!", NULL);
            exception.printError();
          }

          if (valueList.mValueList.getLength() == 0)
            valueList.mForecastTime = *fTime;

          qParam->mValueList.push_back(valueList);
        }
      }
    }



    // Finding out which forecast time are found from the forecast data. The point is that different
    // parameters might contain different forecast times, and we want a list of all forecast times.

    timeList.clear();

    for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
    {
      for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end(); ++it)
      {
        if (timeList.find(it->mForecastTime) == timeList.end())
          timeList.insert(it->mForecastTime);
      }
    }

    // Going through all the found forecast times and making sure that each parameter contains the current
    // forecast time. If not, then the forecast time is added to the parameters's forecast time list, but
    // the actual value list of the current forecast time will be empty.

    query.mForecastTimeList.clear();

    for (auto tt = timeList.begin(); tt != timeList.end(); ++tt)
    {
      query.mForecastTimeList.insert(*tt);

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          bool found = false;
          uint cnt = 0;
          for (auto it = qParam->mValueList.begin(); it != qParam->mValueList.end() &&  !found; ++it)
          {
            if (it->mForecastTime < *tt)
              cnt++;
            else
            if (it->mForecastTime == *tt)
              found = true;
          }

          if (!found)
          {
            // The forecast time was not found from the current parameter. Adding the forecast time
            // with an empty value list.

            ParameterValues pValues;
            pValues.mForecastTime = *tt;
            qParam->mValueList.insert(qParam->mValueList.begin() + cnt,pValues);
          }
        }
      }
    }

    // At this point we have fetched values for all data parameters. Now we are able to
    // execute functions that uses this data.

    executeQueryFunctions(query);
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::AreaInterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  FUNCTION_TRACE
  try
  {
    uint flags = 0;
    uint len = contentInfoList.getLength();
    for (uint c=0; c<len; c++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(c);
      T::ParamValue value = 0;
      int res = mDataServerPtr->getGridValueByPoint(sessionId,contentInfo->mFileId,contentInfo->mMessageIndex,flags,coordinateType,x,y,interpolationMethod,value);
      if (res == DataServer::Result::OK)
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mForecastTime,value));
      }
      else
      {
        valueList.addGridPointValue(new T::GridPointValue(contentInfo->mFileId,contentInfo->mMessageIndex,x,y,contentInfo->mParameterLevel,contentInfo->mForecastTime,ParamValueMissing));
      }
    }
    return 0;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}

