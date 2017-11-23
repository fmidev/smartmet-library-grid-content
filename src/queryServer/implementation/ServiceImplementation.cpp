#include "ServiceImplementation.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GribDef.h>


namespace SmartMet
{
namespace QueryServer
{


ServiceImplementation::ServiceImplementation()
{
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
    std::string gridConfigDirectory,
    string_vec& parameterMappingFiles,
    string_vec& aliasFiles,
    std::string producerFile,
    string_vec& luaFiles)
{
  try
  {
    mContentServerPtr = contentServerPtr;
    mDataServerPtr = dataServerPtr;

    mGridConfigDirectory = gridConfigDirectory;
    SmartMet::Identification::gribDef.init(mGridConfigDirectory.c_str());

    mProducerFile = producerFile;
    loadProducerFile();

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
  try
  {
    // Checking if the configuration files have changed. If so, the
    // changes will be loaded automatically.

    checkConfigurationUpdates();

    //query.print(std::cout,0,0);

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





void ServiceImplementation::loadProducerFile()
{
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
  try
  {
    std::map<T::GeometryId,uint> countList;
    uint maxCount = 0;

    for (auto cList = coordinates.begin(); cList != coordinates.end(); ++cList)
    {
      for (auto coordinate = cList->begin(); coordinate != cList->end(); ++coordinate)
      {
        std::set<T::GeometryId> idList;
        Identification::gribDef.getGeometryIdListByLatLon(coordinate->y(),coordinate->x(),idList);

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
                        std::string forecastTime,
                        bool timeMatchRequired,
                        QueryCoordinates& coordinates,
                        bool areaSearch,
                        ParameterValues& valueList)
{
  try
  {
    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch  &&  coordinates.size() < 3)
    {
      SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 3 coordinates!");
      throw exception;
    }

    // Getting geometries that support support the given coordinates

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(coordinates,geometryIdList);


    // No producers defined. We should go through the producer list that is defined
    // in the configuration file.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;

      T::GeometryId producerGeometryId = it->second;

      // printf("Producer %s:%d\n",producerName.c_str(),producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.
        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,producerName,producerInfo) == 0 &&  (producerId == 0  ||  producerInfo.mProducerId == producerId))
        {
          //producerInfo.print(std::cout,0,0);

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          std::set<std::string> generationNameList;
          uint gLen = generationInfoList.getLength();
          for (uint g=0; g<gLen; g++)
          {
            T::GenerationInfo *gInfo = generationInfoList.getGenerationInfoByIndex(g);
            generationNameList.insert(gInfo->mName);
          }

          for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            m->getMappings(producerInfo.mName,parameterKey,true,mappings);
            printf("*** MAPPINGS %s:%s %u\n",producerInfo.mName.c_str(),parameterKey.c_str(),(uint)mappings.size());

            uint gCount = 0;
            for (auto gName = generationNameList.rbegin(); gName != generationNameList.rend(); gName++)
            {
              uint flags = 1 << gCount;
              if ((generationFlags & flags) != 0)
              {
                T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByName(*gName);

                // We have now a valid generation

                //generationInfo->print(std::cout,0,0);

                for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                {
                  pInfo->print(std::cout,0,0);

                  std:: cout << "**** " << parameterKey << " " << (int)paramLevelId << " " << (int) paramLevel << " " <<  (int)forecastType  << " " << (int)forecastNumber  << "\n";

                  if (paramLevelId > 0 || paramLevel != 0)
                  {
                    pInfo->mParameterLevelId = paramLevelId;
                    pInfo->mParameterLevel = paramLevel;
                  }

                  pInfo->print(std::cout,0,0);

                  T::ContentInfoList contentList;
                  int result = mContentServerPtr->getContentListByParameterGenerationIdAndForecastTime(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,pInfo->mParameterLevelIdType,pInfo->mParameterLevelId,pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,forecastTime,contentList);
                  if (result != 0)
                  {
                    SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                    exception.addParameter("Service","getContentListByParameterGenerationIdAndForecastTime");
                    exception.addParameter("Message",ContentServer::getResultString(result));
                    throw exception;
                  }

                  bool multipleOptions = contentList.containsSameForecastTimes();
                  if (multipleOptions)
                    printf("**** CONTAINS MULTIPLE SAME TIMESTEPS ********\n");

    //            contentList.print(std::cout,0,0);

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
                            int result = mDataServerPtr->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,valueList.mValueList);
                            if (result != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPointList");
                              exception.addParameter("Message",DataServer::getResultString(result));
                              throw exception;
                            }
                          }
                        }
                        else
                        {
                          if (!multipleOptions)
                          {
                            int result = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,valueList.mValueList);
                            if (result != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPolygonPath");
                              exception.addParameter("Message",DataServer::getResultString(result));
                              throw exception;
                            }
                          }
                        }
                        return;
                      }
                      else
                      {
                        SmartMet::Spine::Exception exception(BCP, "Unexpected result!");
                        exception.addDetail("If there is only one content record in place then its forecast time should match to the requested forecast time.");
                        exception.addParameter("Content ForecastTime",contentInfo->mForecastTime);
                        exception.addParameter("Request ForecastTime",forecastTime);
                        throw exception;
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
                            result1 = mDataServerPtr->getGridValueListByPointList(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,list1);
                            if (result1 != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPointList");
                              exception.addParameter("Message",DataServer::getResultString(result1));
                              throw exception;
                            }

                            result2 = mDataServerPtr->getGridValueListByPointList(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,list2);
                            if (result2 != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPointList");
                              exception.addParameter("Message",DataServer::getResultString(result2));
                              throw exception;
                            }
                          }
                        }
                        else
                        {
                          if (!multipleOptions)
                          {
                            result1 = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo1->mFileId,contentInfo1->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,list1);
                            if (result1 != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPointList");
                              exception.addParameter("Message",DataServer::getResultString(result1));
                              throw exception;
                            }

                            result2 = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo2->mFileId,contentInfo2->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,list2);
                            if (result2 != 0)
                            {
                              SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                              exception.addParameter("Service","getGridValueListByPolygonPath");
                              exception.addParameter("Message",DataServer::getResultString(result2));
                              throw exception;
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
                }
              }
              gCount++;
            }
          }
        }
        else
        {
          // printf("Not a valid producer '%s'!\n",producerName.c_str());
        }
      }
      else
      {
        // printf("Producer's '%s' geometry '%d' not supported\n",producerName.c_str(),producerGeometryId);
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
                        std::string startTime,
                        std::string endTime,
                        QueryCoordinates& coordinates,
                        bool areaSearch,
                        ParameterValues_vec& valueList)
{
  try
  {
    if (coordinates.size() == 0)
    {
      SmartMet::Spine::Exception exception(BCP, "No coordinates defined!");
      throw exception;
    }

    if (areaSearch  &&  coordinates.size() < 3)
    {
      SmartMet::Spine::Exception exception(BCP, "Area definition requires at least 3 coordinates!");
      throw exception;
    }

    // Getting geometries that support support the given coordinates

    std::set<T::GeometryId> geometryIdList;
    getGeometryIdListByCoordinates(coordinates,geometryIdList);


    uint requestFlags = (uint)ContentServer::RequestFlags::INCLUDE_TIME_AFTER;


    // No producers defined. We should go through the producer list that is defined
    // in the configuration file.

    for (auto it = producers.begin(); it != producers.end(); ++it)
    {
      std::string producerName = it->first;

      T::GeometryId producerGeometryId = it->second;

      printf("Producer %s:%d\n",producerName.c_str(),producerGeometryId);

      if (geometryIdList.find(producerGeometryId) != geometryIdList.end())
      {
        // The current producer supports a geometry where the current coordinates can be found.
        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,producerName,producerInfo) == 0  &&  (producerId == 0  ||  producerInfo.mProducerId == producerId))
        {
          //producerInfo.print(std::cout,0,0);

          T::GenerationInfoList generationInfoList;
          int result = mContentServerPtr->getGenerationInfoListByProducerId(0,producerInfo.mProducerId,generationInfoList);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
            exception.addParameter("Service","getGenerationInfoListByProducerId");
            exception.addParameter("Message",ContentServer::getResultString(result));
            throw exception;
          }

          std::set<std::string> generationNameList;
          uint gLen = generationInfoList.getLength();
          for (uint g=0; g<gLen; g++)
          {
            T::GenerationInfo *gInfo = generationInfoList.getGenerationInfoByIndex(g);
            generationNameList.insert(gInfo->mName);
          }

          for (auto m = mParameterMappings.begin(); m != mParameterMappings.end(); ++m)
          {
            ParameterMapping_vec mappings;
            m->getMappings(producerInfo.mName,parameterKey,true,mappings);
            printf("*** MAPPINGS %s:%s %u\n",producerInfo.mName.c_str(),parameterKey.c_str(),(uint)mappings.size());


            uint gCount = 0;
            for (auto gName = generationNameList.rbegin(); gName != generationNameList.rend(); gName++)
            {
              uint flags = 1 << gCount;
              if ((generationFlags & flags) != 0)
              {
                T::GenerationInfo *generationInfo = generationInfoList.getGenerationInfoByName(*gName);

                //generationInfo->print(std::cout,0,0);

                for (auto pInfo = mappings.begin(); pInfo != mappings.end(); ++pInfo)
                {
                  pInfo->print(std::cout,0,0);

                  if (paramLevelId > 0 || paramLevel != 0)
                  {
                    pInfo->mParameterLevelId = paramLevelId;
                    pInfo->mParameterLevel = paramLevel;
                  }

                  pInfo->print(std::cout,0,0);

                  T::ContentInfoList contentList;
                  int result = mContentServerPtr->getContentListByParameterAndGenerationId(0,generationInfo->mGenerationId,pInfo->mParameterKeyType,pInfo->mParameterKey,pInfo->mParameterLevelIdType,pInfo->mParameterLevelId,pInfo->mParameterLevel,pInfo->mParameterLevel,forecastType,forecastNumber,producerGeometryId,startTime,endTime,requestFlags,contentList);
                  if (result != 0)
                  {
                    SmartMet::Spine::Exception exception(BCP, "ContentServer returns an error!");
                    exception.addParameter("Service","getContentListByParameterAndGenerationId");
                    exception.addParameter("Message",ContentServer::getResultString(result));
                    throw exception;
                  }

                  contentList.print(std::cout,0,0);

                  bool multipleOptions = contentList.containsSameForecastTimes();
                  if (multipleOptions)
                  {
                    printf("**** CONTAINS MULTIPLE SAME TIMESTEPS ********\n");
                    contentList.print(std::cout,0,0);
                  }

                  std::string lastTime = startTime;

                  uint clen = contentList.getLength();

                  for (uint t=0; t<clen; t++)
                  {
                    T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(t);
                    //printf("%s:%d:%s\n",pInfo->mParameterKey.c_str(),contentInfo->mGeometryId,contentInfo->mForecastTime.c_str());

                    if (contentInfo->mForecastTime >= startTime  &&  contentInfo->mForecastTime <= endTime)
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
                          result = mDataServerPtr->getGridValueListByPointList(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates[0],pInfo->mInterpolationMethod,valList.mValueList);
                          if (result != 0)
                          {
                            SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                            exception.addParameter("Service","getGridValueListByPointList");
                            exception.addParameter("Message",DataServer::getResultString(result));
                            exception.printError();
                            //throw exception;
                          }
                        }
                      }
                      else
                      {
                        if (!multipleOptions)
                        {
                          result = mDataServerPtr->getGridValueListByPolygonPath(0,contentInfo->mFileId,contentInfo->mMessageIndex,T::CoordinateType::LATLON_COORDINATES,coordinates,valList.mValueList);
                          if (result != 0)
                          {
                            SmartMet::Spine::Exception exception(BCP, "DataServer returns an error!");
                            exception.addParameter("Service","getGridValueListByPolygonPath");
                            exception.addParameter("Message",DataServer::getResultString(result));
                            exception.printError();
                            //throw exception;
                          }
                        }
                      }

                      if (result == 0)
                      {
                        if (contentInfo->mForecastTime > lastTime)
                          lastTime = contentInfo->mForecastTime;

                        //printf("*** VALUES ****\n");
                        //valList.print(std::cout,0,0);
                        valueList.push_back(valList);
                      }
                    }

                    if (contentInfo->mForecastTime >= endTime)
                    {
                      // We have found the full time range from this generation, so we
                      // should end the search. It is good to notice that the forecast
                      // time of the last content is not necessary the same as the exact
                      // end time.

                      if (!multipleOptions)
                        return;
                    }
                  }

                  if (lastTime >= endTime)
                    if (!multipleOptions)
                      return;

                  // We have not found content for the full time range, so we should continue to search.

                  if (lastTime > startTime)
                  {
                    if (!multipleOptions)
                    {
                      boost::posix_time::ptime tt = toTimeStamp(lastTime) + boost::posix_time::minutes(1);
                      startTime = toString(tt);
                    }
                  }
                }
              }
              gCount++;
            }
          }
        }
        else
        {
           printf("Not a valid producer '%s'!\n",producerName.c_str());
        }
      }
      else
      {
        printf("Producer's '%s' geometry '%d' not supported\n",producerName.c_str(),producerGeometryId);
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





void ServiceImplementation::getParameterStringInfo(std::string param,std::string& key,T::ParamLevelId& paramLevelId,T::ParamLevel& paramLevel,T::ForecastType& forecastType,T::ForecastNumber& forecastNumber,uint& producerId,uint& generationFlags)
{
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
        paramLevelId = (T::ParamLevelId)atoi(field[1]);
    }

    if (c > 2)
    {
      if (field[2][0] != '\0')
        paramLevel = atoi(field[2]);
    }

    if (c > 3)
    {
      if (field[3][0] != '\0')
        forecastType = (T::ForecastType)atoi(field[3]);
    }

    if (c > 4)
    {
      if (field[4][0] != '\0')
        forecastNumber = (T::ForecastNumber)atoi(field[4]);
    }

    if (c > 5)
    {
      if (field[5][0] != '\0')
      {
        T::ProducerInfo producerInfo;
        if (mContentServerPtr->getProducerInfoByName(0,std::string(field[5]),producerInfo) == 0)
          producerId = producerInfo.mProducerId;
      }
    }
    if (c > 6)
    {
      if (field[6][0] != '\0')
        generationFlags = atoi(field[6]);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





bool ServiceImplementation::getAlias(std::string name,std::string& alias)
{
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

    printf("PARSE FUNCTION %s\n",paramStr.c_str());
    std::string functionParamsStr;
    if (getParameterFunctionInfo(paramStr,function,functionParamsStr))
    {
      printf("PARSE %s => %s  %s\n",paramStr.c_str(),function.c_str(),functionParamsStr.c_str());
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
          newParam.mInterpolationMethod = queryParam.mInterpolationMethod;
          newParam.mProducerId = queryParam.mProducerId;
          newParam.mGenerationFlags = queryParam.mGenerationFlags;
          newParam.mTemporary = true;

          std::string alias;
          if (getAlias(fParam->second,alias))
          {
            newParam.mParam = alias;
            newParam.mOrigParam = newParam.mOrigParam + " => " + alias;
            std::string paramName;
            getParameterStringInfo(fParam->second,paramName,newParam.mParameterLevelId,newParam.mParameterLevel,newParam.mForecastType,newParam.mForecastNumber,newParam.mProducerId,newParam.mGenerationFlags);

          }
          newParam.print(std::cout,0,0);

          parseFunction(newParam,newParam.mParam,newParam.mFunction,newParam.mFunctionParams,recursionCounter+1,additionalParameterList);

          additionalParameterList.insert(additionalParameterList.begin(),newParam);
        }
      }
      return true;
    }
    else
    {
      std::string paramName;
      getParameterStringInfo(paramStr,paramName,queryParam.mParameterLevelId,queryParam.mParameterLevel,queryParam.mForecastType,queryParam.mForecastNumber,queryParam.mProducerId,queryParam.mGenerationFlags);

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





double ServiceImplementation::executeFunctionType1(std::string& function,std::vector<double>& parameters)
{
  try
  {
    return mLuaFileCollection.executeFunctionType1(function,parameters);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::checkConfigurationUpdates()
{
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
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::getProducerList(Query& query,Producer_vec& producers)
{
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
          producers.push_back(std::pair<std::string,T::GeometryId>(it->first,it->second));
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
  try
  {
    bool updateRequired = true;
    uint updateCounter = 0;

    while (updateRequired  &&  updateCounter < 10)
    {
      updateRequired = false;
      updateCounter++;

      std::vector<QueryParameter> additionalParameterList;

      // Going through all the query parameters.

      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        // The parameter can contain some level information even if the parameter is an alias
        // or a function. In this case this level information should be inherited to the alias
        // definition or to the function parameters.

        getParameterStringInfo(qParam->mParam,qParam->mParameterKey,qParam->mParameterLevelId,qParam->mParameterLevel,qParam->mForecastType,qParam->mForecastNumber,qParam->mProducerId,qParam->mGenerationFlags);

        // If the parameter is a symbolic name (defined in the configuration file) then
        // we should use its definition.

        std::string alias;
        if (getAlias(qParam->mParameterKey,alias))
        {
          std::cout << qParam->mSymbolicName << "  => " << alias << " ( " << qParam->mParam << " )\n";
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
    }

    //query.print(std::cout,0,0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





void ServiceImplementation::executeQueryFunctions(Query& query)
{
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
                    a = rec->mValue;
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

              parameters.push_back(a);
              areaParameters.push_back(a);

              if (areaCnt  &&  v == 0)
                extParameters.push_back(a);
            }

            if (qParam->mFunction.substr(0,1) != "@")
            {
              double val = executeFunctionType1(qParam->mFunction,parameters);
              pValues.mValueList.addGridValue(new T::GridValue(lastRec.mX,lastRec.mY,(T::ParamValue)val));
              pValues.mProducerId = producerId;
              pValues.mGenerationId = generationId;
              pValues.mGeometryId = geometryId;
            }
          }

          if (areaCnt  &&  qParam->mFunction.substr(0,1) != "@")
          {
            double val = executeFunctionType1(qParam->mFunction,extParameters);
            pValues.mValueList.addGridValue(new T::GridValue(-1000,-1000,(T::ParamValue)val));
          }

          if (qParam->mFunction.substr(0,1) == "@")
          {
            //T::ParamValue val = executeAreaFunction(qParam->mFunction,areaParameters);
            std::string func = qParam->mFunction.substr(1);
            double val = executeFunctionType1(func,areaParameters);
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

    query.print(std::cout,0,0);

    query.removeTemporaryParameters();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP, "Operation failed!", NULL);
  }
}





int ServiceImplementation::executeTimeRangeQuery(Query& query)
{
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

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
        uint producerId = qParam->mProducerId;
        uint generationFlags = qParam->mGenerationFlags;
        if (generationFlags == 0)
          generationFlags = query.mGenerationFlags;

        //qParam->print(std::cout,0,0);

        getParameterStringInfo(qParam->mParam,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,producerId,generationFlags);

        if (paramName.substr(0,1) == "$")
          paramName = paramName.substr(1);

        try
        {
          if (qParam->mParameterKeyType != T::ParamKeyType::BUILD_IN)
            getGridValues(producers,producerId,generationFlags,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,query.mStartTime,query.mEndTime,query.mCoordinateList,areaQuery,qParam->mValueList);
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
      query.mForecastTimeList.push_back(*tt);

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
  try
  {
    // Fetching valid producers.

    Producer_vec producers;
    getProducerList(query,producers);

    // Parsing parameters and functions in the query.

    updateQueryParameters(query);

    bool areaQuery = false;
    if ((query.mFlags & QF_AREA_QUERY) != 0)
      areaQuery = true;

    // Fetching parameter data according to the given timesteps and the coordinate list. Notice
    // that the coordinate list can be used in two ways. It can 1) contain coordinate points
    // where the data will be fetched or 2) it can define an area (= polygon) where the grid
    // points are fetched. The area can be defined by using single or multiple polygons.
    // That's why the coordinates are defined as a vector of coordinate vectors.

    for (auto fTime = query.mForecastTimeList.begin(); fTime != query.mForecastTimeList.end(); ++fTime)
    {
      for (auto qParam = query.mQueryParameterList.begin(); qParam != query.mQueryParameterList.end(); ++qParam)
      {
        if (qParam->mFunction.length() == 0)
        {
          std::string paramName = qParam->mParam;
          T::ParamLevelId paramLevelId = qParam->mParameterLevelId;
          T::ParamLevel paramLevel = qParam->mParameterLevel;
          T::ForecastType forecastType = qParam->mForecastType;
          T::ForecastNumber forecastNumber = qParam->mForecastNumber;
          uint producerId = qParam->mProducerId;
          uint generationFlags = qParam->mGenerationFlags;
          if (generationFlags == 0)
            generationFlags = query.mGenerationFlags;

          getParameterStringInfo(qParam->mParam,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,producerId,generationFlags);

          if (paramName.substr(0,1) == "$")
            paramName = paramName.substr(1);

          ParameterValues valueList;
          try
          {
            if (qParam->mParameterKeyType != T::ParamKeyType::BUILD_IN)
              getGridValues(producers,producerId,generationFlags,paramName,paramLevelId,paramLevel,forecastType,forecastNumber,*fTime,false,query.mCoordinateList,areaQuery,valueList);
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





int ServiceImplementation::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  try
  {
    uint len = contentInfoList.getLength();
    for (uint c=0; c<len; c++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(c);
      T::ParamValue value = 0;
      int res = mDataServerPtr->getGridValueByPoint(sessionId,contentInfo->mFileId,contentInfo->mMessageIndex,coordinateType,x,y,interpolationMethod,value);
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

