#pragma once

#include "ContentInfo.h"
#include "grid-files/common/ModificationLock.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class ContentInfoList
{
  public:
                      ContentInfoList();
                      ContentInfoList(ContentInfoList& contentInfoList);
     virtual          ~ContentInfoList();

     void             operator=(ContentInfoList& contentInfoList);

     void             addContentInfo(ContentInfo *contentInfo);
     void             clear();
     uint             deleteContentInfoByFileId(uint fileId);
     uint             deleteContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex);
     uint             deleteContentInfoByGroupFlags(uint groupFlags);
     uint             deleteContentInfoByProducerId(uint producerId);
     uint             deleteContentInfoByGenerationId(uint generationId);
     uint             registerContentInfoByServerAndFileId(uint serverId,uint fileId);
     uint             unregisterContentInfoByServerId(uint serverId);
     uint             unregisterContentInfoByServerAndFileId(uint serverId,uint fileId);
     ContentInfo*     getContentInfoByIndex(uint index);
     ContentInfo*     getContentInfoByIndexNoCheck(uint index);
     ContentInfo*     getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex);
     bool             getContentInfoByFileIdAndMessageIndex(uint fileId,uint messageIndex,ContentInfo& contentInfo);
     void             getContentInfoList(uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
     void             getContentInfoListByGroupFlags(uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
     void             getContentInfoListByFileId(uint fileId,ContentInfoList& contentInfoList);

     void             getContentInfoListByGribParameterId(T::ParamId gribParameterId,ContentInfoList& contentInfoList);
     void             getContentInfoListByGribParameterId(T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByGribParameterIdAndGenerationId(uint generationId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByGribParameterIdAndProducerId(uint producerId,T::ParamId gribParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterId(T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterIdAndGenerationId(uint generationId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterIdAndProducerId(uint producerId,T::ParamId fmiParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterName(std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterNameAndGenerationId(uint generationId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByFmiParameterNameAndProducerId(uint producerId,std::string fmiParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterId(T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterIdAndGenerationId(uint generationId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterIdAndProducerId(uint producerId,T::ParamId newbaseParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterName(std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterNameAndGenerationId(uint generationId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByNewbaseParameterNameAndProducerId(uint producerId,std::string newbaseParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterId(T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterIdAndGenerationId(uint generationId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterIdAndProducerId(uint producerId,T::ParamId cdmParameterId,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterName(std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterNameAndGenerationId(uint generationId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);
     void             getContentInfoListByCdmParameterNameAndProducerId(uint producerId,std::string cdmParameterName,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,ContentInfoList& contentInfoList);

     void             getContentInfoListByGenerationId(uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
     void             getContentInfoListByGenerationId(uint generationId,std::string startTime,std::string endTime,ContentInfoList& contentInfoList);
     void             getContentInfoListByProducerId(uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
     void             getContentInfoListByServerId(uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,ContentInfoList& contentInfoList);
     void             getContentInfoListByServerAndFileId(uint serverId,uint fileId,ContentInfoList& contentInfoList);
     int              getClosestIndex(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo);
     uint             getLength();
     uint             getSize();
     bool             getReleaseObjects();
     void             print(std::ostream& stream,uint level,uint optionFlags);
     void             increaseSize(uint newSize);
     void             lock();
     void             unlock();

     void             setComparisonMethod(ContentInfo::ComparisonMethod comparisonMethod);
     void             setReleaseObjects(bool releaseObjects);
     void             sort(ContentInfo::ComparisonMethod comparisonMethod);

     void             writeToFile(std::string filename);
     void             writeToFile(std::string filename,const char *filemode);

  protected:

     int              getClosestIndexNoLock(ContentInfo::ComparisonMethod comparisonMethod,ContentInfo& contentInfo);

     ContentInfoPtr   *mArray;
     uint             mSize;
     uint             mLength;
     ModificationLock mModificationLock;
     bool             mReleaseObjects;

     ContentInfo::ComparisonMethod mComparisonMethod;
};



}
}
