#pragma once

#include "ContentInfo.h"
#include "GenerationInfoList.h"
#include "ParameterLevelInfoList.h"
#include "LevelInfoList.h"

#include <grid-files/common/ModificationLock.h>
#include <vector>
#include <set>


namespace SmartMet
{
namespace T
{


class ContentInfoList
{
  public:
                        ContentInfoList();
                        ContentInfoList(ContentInfoList& contentInfoList);
                        ContentInfoList(const ContentInfoList& contentInfoList);
    virtual             ~ContentInfoList();

    ContentInfoList&    operator=(ContentInfoList& contentInfoList);
    ContentInfoList&    operator=(const ContentInfoList& contentInfoList);

    ContentInfo*        addContentInfo(ContentInfo *contentInfo);
    void                addContentInfoList(ContentInfoList& contentInfoList);
    void                addContentInfoListNoLock(ContentInfoList& contentInfoList);

    void                clear();

    bool                containsSameForecastTimes();

    uint                deleteContentInfo(ContentInfo& _contentInfo);
    uint                deleteContentInfoByFileId(uint fileId);
    uint                deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex);
    uint                deleteContentInfoByProducerId(uint producerId);
    uint                deleteContentInfoByGenerationId(uint generationId);
    uint                deleteContentInfoByGenerationIdList(std::set<uint>& generationIdList);
    uint                deleteContentInfoByGenerationAndGeometry(uint generationId,T::GeometryId geometryId);
    uint                deleteContentInfoByGenerationGeometryAndForecastTime(uint generationId,T::GeometryId geometryId,const std::string& forecastTime);
    uint                deleteContentInfoByGenerationGeometryAndForecastTime(uint generationId,T::GeometryId geometryId,time_t forecastTimeUTC);
    uint                deleteContentInfoBySourceId(uint sourceId);
    uint                deleteContentInfoByFileIdList(std::set<uint>& fileIdList);
    uint                deleteMarkedContent();
    uint                deleteVirtualContent();
    uint                markDeleted();
    uint                markDeletedByVirtualFlag();
    uint                markDeletedByFileId(uint fileId);
    uint                markDeletedByFileIdAndMessageIndex(uint fileId,uint messageIndex);
    uint                markDeletedByGenerationId(uint generationId);
    uint                markDeletedByProducerId(uint fileId);
    uint                markDeletedBySourceId(uint sourceId);

    void                keepContentInfoByGeometryIdList(std::set<T::GeometryId>& geometryIdList);
    void                keepContentInfoByGeometryId(T::GeometryId geometryId);
    void                keepContentInfoByProducerId(uint producerId);
    void                keepContentInfoByGenerationId(uint generationId);
    void                keepContentInfoBySourceId(uint sourceId);

    ContentInfo*        getContentInfoByIndex(uint index) const;
    ContentInfo*        getContentInfoByIndexNoCheck(uint index);
    ContentInfo*        getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex);
    ContentInfo*        getContentInfoByFileIdAndMessageIndexNoLock(uint fileId,uint messageIndex);
    bool                getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo);
    ContentInfo*        getContentInfoByParameterLevelInfo(T::ParameterLevelInfo& levelInfo);

    ContentInfo*        getContentInfoByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime);
    ContentInfo*        getContentInfoByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC);
    ContentInfo*        getContentInfoByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime);
    ContentInfo*        getContentInfoByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC);

    void                getContentInfoListByParameterLevelInfo(T::ParameterLevelInfo& levelInfo,ContentInfoList& contentInfoList);
    void                getContentInfoList(uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList);
    void                getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList);
    void                getContentInfoListByForecastTime(const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByForecastTime(time_t forecastTimeUTC,ContentInfoList& contentInfoList);

    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(uint producerId,uint generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);

    void                getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId2(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId2(uint producerId,uint generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationList(T::GenerationInfoList& generationList,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndProducerId(uint producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndProducerId(uint producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);

    void                getContentInfoListByRequestCounterKey(ulonglong key,ContentInfoList& contentInfoList);

    void                getContentListByForecastTime(const std::string& forecastTime,T::ContentInfoList& contentInfoList);
    void                getContentListByForecastTime(time_t forecastTimeUTC,T::ContentInfoList& contentInfoList);

    void                getContentInfoListByGenerationId(uint producerId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(uint producerId,uint generationId,const std::string& startTime,const std::string& endTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(uint producerId,uint generationId,time_t startTimeUTC,time_t endTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationAndGeometryId(uint producerId,uint generationId,T::GeometryId geometryId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(uint producerId,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
    int                 getClosestIndex(uint comparisonMethod,ContentInfo& contentInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,ContentInfo& contentInfo);
    void                getContentInfoListBySourceId(uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);

    void                getContentGeometryIdList(std::set<T::GeometryId>& geometryIdList);
    void                getContentGeometryIdListByGenerationId(uint producerId,uint generationId,std::set<T::GeometryId>& geometryIdList);
    void                getContentParamKeyListByGenerationId(uint producerId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    void                getGenerationIdListByGeometryId(T::GeometryId geometryId,std::set<uint>& generationIdList);

    void                getForecastTimeList(std::set<std::string>& forecastTimeList);
    void                getForecastTimeList(std::set<time_t>& forecastTimeList);
    void                getForecastTimeListByGenerationId(uint producerId,uint generationId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByGenerationAndGeometry(uint producerId,uint generationId,T::GeometryId geometryId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByProducerId(uint producerId,std::set<std::string>& forecastTimeList);
    void                getFmiParamLevelIdListByFmiParameterId(T::FmiParamId fmiParameterId,std::vector<T::ParamLevelId>& paramLevelIdList);
    void                getParamLevelListByFmiLevelId(T::ParamLevelId paramLevelId,std::set<T::ParamLevel>& paramLevelList);
    void                getParamLevelInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ParameterLevelInfoList& parameterLevelInfoList);

    void                getLevelInfoList(T::LevelInfoList& levelInfoList);
    std::size_t         getHash();
    std::size_t         getHashByProducerId(uint producerId);

    uint                getLength()const;
    ModificationLock*   getModificationLockPtr();
    void                setModificationLockPtr(ModificationLock* modificationLockPtr);

    uint                getSize() const;
    bool                getReleaseObjects();
    void                print(std::ostream& stream,uint level,uint optionFlags);
    void                lock();
    void                unlock();

    uint                getComparisonMethod();
    void                setComparisonMethod(uint comparisonMethod);
    void                setLockingEnabled(bool lockingEnabled);
    void                setReleaseObjects(bool releaseObjects);
    void                sort(uint comparisonMethod);

    void                writeToFile(const std::string& filename);
    void                writeToFile(const std::string& filename,const char *filemode);

  protected:

    void                increaseSize(uint newSize);

    ContentInfoPtr      *mArray;
    uint                mSize;
    uint                mLength;
    ModificationLock    mModificationLock;
    ModificationLock*   mModificationLockPtr;
    bool                mReleaseObjects;
    uint                mComparisonMethod;
};



}
}
