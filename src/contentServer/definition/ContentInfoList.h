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
    uint                deleteContentInfoByFileId(T::FileId fileId);
    uint                deleteContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    uint                deleteContentInfoByProducerId(T::ProducerId producerId);
    uint                deleteContentInfoByGenerationId(T::GenerationId generationId);
    uint                deleteContentInfoByGenerationIdList(std::set<T::GenerationId>& generationIdList);
    uint                deleteContentInfoByGenerationAndGeometry(T::GenerationId generationId,T::GeometryId geometryId);
    uint                deleteContentInfoByGenerationGeometryAndForecastTime(T::GenerationId generationId,T::GeometryId geometryId,const std::string& forecastTime);
    uint                deleteContentInfoByGenerationGeometryAndForecastTime(T::GenerationId generationId,T::GeometryId geometryId,time_t forecastTimeUTC);
    uint                deleteContentInfoBySourceId(T::SourceId sourceId);
    uint                deleteContentInfoByStorageId(T::StorageId storageId);
    uint                deleteContentInfoByFileIdList(std::set<T::FileId>& fileIdList);
    uint                deleteMarkedContent();
    uint                markDeleted();
    uint                markDeletedByFileId(T::FileId fileId);
    uint                markDeletedByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    uint                markDeletedByGenerationId(T::GenerationId generationId);
    uint                markDeletedByGenerationAndGeometryId(T::GenerationId generationId,T::GeometryId geometryId);
    uint                markDeletedByProducerId(T::ProducerId producerId);
    uint                markDeletedBySourceId(T::SourceId sourceId);
    uint                markDeletedByStorageId(T::StorageId storageId);

    void                keepContentInfoByGeometryIdList(std::set<T::GeometryId>& geometryIdList);
    void                keepContentInfoByGeometryId(T::GeometryId geometryId);
    void                keepContentInfoByProducerId(T::ProducerId producerId);
    void                keepContentInfoByGenerationId(T::GenerationId generationId);
    void                keepContentInfoBySourceId(T::SourceId sourceId);
    void                keepContentInfoByStorageId(T::StorageId storageId);

    ContentInfo*        getContentInfoByIndex(uint index) const;
    ContentInfo*        getContentInfoByIndexNoCheck(uint index);
    ContentInfo*        getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex);
    ContentInfo*        getContentInfoByFileIdAndMessageIndexNoLock(T::FileId fileId,T::MessageIndex messageIndex);
    bool                getContentInfoByFileIdAndMessageIndex(T::FileId fileId,T::MessageIndex messageIndex,ContentInfo& contentInfo);
    ContentInfo*        getContentInfoByParameterLevelInfo(T::ParameterLevelInfo& levelInfo);

    ContentInfo*        getContentInfoByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime);
    ContentInfo*        getContentInfoByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC);
    ContentInfo*        getContentInfoByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime);
    ContentInfo*        getContentInfoByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC);

    void                getContentInfoListByParameterLevelInfo(T::ParameterLevelInfo& levelInfo,ContentInfoList& contentInfoList);
    void                getContentInfoList(ContentInfoList& contentInfoList);
    void                getContentInfoListNoClear(ContentInfoList& contentInfoList);
    void                getContentInfoList(T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListNoClear(T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGeometryId(T::GeometryId geometryId,ContentInfoList& contentInfoList);
    void                getContentInfoListByFileId(T::FileId fileId,ContentInfoList& contentInfoList);
    void                getContentInfoListByForecastTime(const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByForecastTime(time_t forecastTimeUTC,ContentInfoList& contentInfoList);

    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterId(T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId2(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndGenerationId2(T::ProducerId producerId,T::GenerationId generationId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndProducerId(T::ProducerId producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterIdAndProducerId(T::ProducerId producerId,T::FmiParamId fmiParameterId,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);

    void                getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterName(const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId2(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationId2(T::ProducerId producerId,T::GenerationId generationId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t forecastTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndGenerationList(T::GenerationInfoList& generationList,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndProducerId(T::ProducerId producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,ContentInfoList& contentInfoList);
    void                getContentInfoListByFmiParameterNameAndProducerId(T::ProducerId producerId,const std::string& fmiParameterName,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,time_t startTimeUTC,time_t endTimeUTC,uint requestFlags,ContentInfoList& contentInfoList);

    //void                getContentInfoListByRequestCounterKey(UInt64 key,ContentInfoList& contentInfoList);

    void                getContentListByForecastTime(const std::string& forecastTime,T::ContentInfoList& contentInfoList);
    void                getContentListByForecastTime(time_t forecastTimeUTC,T::ContentInfoList& contentInfoList);

    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,const std::string& startTime,const std::string& endTime,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t startTimeUTC,time_t endTimeUTC,ContentInfoList& contentInfoList);
    void                getContentInfoListByGenerationAndGeometryId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(T::ProducerId producerId,ContentInfoList& contentInfoList);
    void                getContentInfoListByProducerId(T::ProducerId producerId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);
    int                 getClosestIndex(uint comparisonMethod,ContentInfo& contentInfo);
    int                 getClosestIndexNoLock(uint comparisonMethod,ContentInfo& contentInfo);
    void                getContentInfoListBySourceId(T::SourceId sourceId,T::FileId startFileId,T::MessageIndex startMessageIndex,int maxRecords,ContentInfoList& contentInfoList);

    void                getContentGeometryIdList(std::set<T::GeometryId>& geometryIdList);
    void                getContentGeometryIdListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,std::set<T::GeometryId>& geometryIdList);
    void                getContentParamKeyListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    void                getContentParamKeyListByGenerationAndGeometryId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    void                getContentParamKeyListByGenerationGeometryAndLevelId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList);
    void                getGenerationIdListByGeometryId(T::GeometryId geometryId,std::set<T::GenerationId>& generationIdList);

    void                getContentLevelListByGenerationGeometryAndLevelId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);
    void                getContentLevelListByParameterGenerationGeometryAndLevelId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,std::string& parameterKey,T::ParamLevelId levelId,std::set<T::ParamLevel>& contentLevelList);

    void                getForecastTimeList(std::set<std::string>& forecastTimeList);
    void                getForecastTimeList(std::set<time_t>& forecastTimeList);
    void                getForecastTimeListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByGenerationId(T::ProducerId producerId,T::GenerationId generationId,std::set<time_t>& forecastTimeList);
    void                getForecastTimeListByGenerationAndGeometry(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByGenerationAndGeometry(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,std::set<time_t>& forecastTimeList);
    void                getForecastTimeListByGenerationGeometryAndLevelId(T::ProducerId producerId,T::GenerationId generationId,T::GeometryId geometryId,T::ParamLevelId levelId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByProducerId(T::ProducerId producerId,std::set<std::string>& forecastTimeList);
    void                getForecastTimeListByProducerId(T::ProducerId producerId,std::set<time_t>& forecastTimeList);
    void                getForecastTimeRangeByGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime);
    void                getForecastTimeRangeByGenerationId(T::ProducerId producerId,T::GenerationId generationId,time_t& startTime,time_t& endTime,std::size_t& hash);

    void                getFmiParamLevelIdListByFmiParameterId(T::FmiParamId fmiParameterId,std::vector<T::ParamLevelId>& paramLevelIdList);
    void                getParamLevelListByFmiLevelId(T::ParamLevelId paramLevelId,std::set<T::ParamLevel>& paramLevelList);
    void                getParamLevelInfoListByFmiParameterId(T::FmiParamId fmiParameterId,ParameterLevelInfoList& parameterLevelInfoList);

    void                getLevelInfoList(T::LevelInfoList& levelInfoList);
    std::size_t         getHash();
    std::size_t         getHashByProducerId(T::ProducerId producerId);

    uint                getLength()const;
    ModificationLock*   getModificationLockPtr();
    void                setModificationLockPtr(ModificationLock* modificationLockPtr);

    uint                getSize() const;
    void                setSize(uint newSize);
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
