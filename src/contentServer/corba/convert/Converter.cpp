#include "Converter.h"
#include <macgyver/Exception.h>

namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{



Converter::Converter()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





Converter::~Converter()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void Converter::convert(T::ContentInfo& source,C::ContentInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileType = (CORBA::Octet)source.mFileType;
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.filePosition = source.mFilePosition;
    target.messageSize = source.mMessageSize;
    target.forecastTime = CORBA::string_dup(source.getForecastTime());
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.parameterId = source.mFmiParameterId;
    target.parameterName = CORBA::string_dup(source.getFmiParameterName());
    target.levelId = source.mFmiParameterLevelId;
    target.level = source.mParameterLevel;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.geometryId = source.mGeometryId;
    target.modificationTime = source.mModificationTime;
    target.deletionTime = source.mDeletionTime;
    target.aggregationId = source.mAggregationId;
    target.aggregationPeriod = source.mAggregationPeriod;
    target.processingTypeId = source.mProcessingTypeId;
    target.processingTypeValue1 = source.mProcessingTypeValue1;
    target.processingTypeValue2 = source.mProcessingTypeValue2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ContentInfo& source,T::ContentInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileType = source.fileType;
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mFilePosition = source.filePosition;
    target.mMessageSize = source.messageSize;
    target.setForecastTime(source.forecastTime);
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mFmiParameterId = source.parameterId;
    target.setFmiParameterName(source.parameterName);
    target.mFmiParameterLevelId = source.levelId;
    target.mParameterLevel = source.level;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mGeometryId = source.geometryId;
    target.mModificationTime = source.modificationTime;
    target.mDeletionTime = source.deletionTime;
    target.mAggregationId = source.aggregationId;
    target.mAggregationPeriod = source.aggregationPeriod;
    target.mProcessingTypeId = source.processingTypeId;
    target.mProcessingTypeValue1 = source.processingTypeValue1;
    target.mProcessingTypeValue2 = source.processingTypeValue2;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::ContentInfoList& source,C::ContentInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = source.getContentInfoByIndex(t);
      C::ContentInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ContentInfoList& source,T::ContentInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::ContentInfo corbaObject = source[t];
      T::ContentInfo *info = new T::ContentInfo();
      convert(corbaObject,*info);
      target.addContentInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void Converter::convert(T::FileInfo& source,C::FileInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileId = source.mFileId;
    target.protocol = (CORBA::Octet)source.mProtocol;
    target.server = CORBA::string_dup(source.mServer.c_str());
    target.fileType = (CORBA::Octet)source.mFileType;
    target.name = CORBA::string_dup(source.mName.c_str());
    target.size = source.mSize;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.modificationTime = source.mModificationTime;
    target.deletionTime = source.mDeletionTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::FileInfo& source,T::FileInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileId = source.fileId;
    target.mProtocol = source.protocol;
    target.mServer = source.server;
    target.mFileType = source.fileType;
    target.mName = source.name;
    target.mSize = source.size;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mModificationTime = source.modificationTime;
    target.mDeletionTime = source.deletionTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::FileInfoList& source,C::FileInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *info = source.getFileInfoByIndex(t);
      C::FileInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::FileInfoList& source,T::FileInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::FileInfo corbaObject = source[t];
      T::FileInfo *info = new T::FileInfo();
      convert(corbaObject,*info);
      target.addFileInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::ProducerInfo& source,C::ProducerInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.name = CORBA::string_dup(source.mName.c_str());
    target.title = CORBA::string_dup(source.mTitle.c_str());
    target.description = CORBA::string_dup(source.mDescription.c_str());
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.hash = source.mHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ProducerInfo& source,T::ProducerInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mName = source.name;
    target.mTitle = source.title;
    target.mDescription = source.description;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mHash = source.hash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::ProducerInfoList& source,C::ProducerInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *info = source.getProducerInfoByIndex(t);
      C::ProducerInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::ProducerInfoList& source,T::ProducerInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::ProducerInfo corbaObject = source[t];
      T::ProducerInfo *info = new T::ProducerInfo();
      convert(corbaObject,*info);
      target.addProducerInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(T::GenerationInfo& source,C::GenerationInfo& target)
{
  try
  {
    target.generationId = source.mGenerationId;
    target.generationType = source.mGenerationType;
    target.producerId = source.mProducerId;
    target.name = CORBA::string_dup(source.mName.c_str());
    target.description = CORBA::string_dup(source.mDescription.c_str());
    target.analysisTime = CORBA::string_dup(source.mAnalysisTime.c_str());
    target.status = (CORBA::Octet)source.mStatus;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.deletionTime = source.mDeletionTime;
    target.modificationTime = source.mModificationTime;
    target.contentStartTime = source.mContentStartTime;
    target.contentEndTime = source.mContentEndTime;
    target.contentHash = source.mContentHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GenerationInfo& source,T::GenerationInfo& target)
{
  try
  {
    target.mGenerationId = source.generationId;
    target.mGenerationType = source.generationType;
    target.mProducerId = source.producerId;
    target.mName = source.name;
    target.mDescription = source.description;
    target.mAnalysisTime = source.analysisTime;
    target.mStatus = source.status;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mDeletionTime = source.deletionTime;
    target.mModificationTime = source.modificationTime;
    target.mContentStartTime = source.contentStartTime;
    target.mContentEndTime = source.contentEndTime;
    target.mContentHash = source.contentHash;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::GenerationInfoList& source,C::GenerationInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *info = source.getGenerationInfoByIndex(t);
      C::GenerationInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GenerationInfoList& source,T::GenerationInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::GenerationInfo corbaObject = source[t];
      T::GenerationInfo *info = new T::GenerationInfo();
      convert(corbaObject,*info);
      target.addGenerationInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::GeometryInfo& source,C::GeometryInfo& target)
{
  try
  {
    target.generationId = source.mGenerationId;
    target.geometryId = source.mGeometryId;
    target.levelId = source.mLevelId;
    target.producerId = source.mProducerId;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.deletionTime = source.mDeletionTime;
    target.modificationTime = source.mModificationTime;
    target.status = (CORBA::Octet)source.mStatus;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GeometryInfo& source,T::GeometryInfo& target)
{
  try
  {
    target.mGenerationId = source.generationId;
    target.mGeometryId = source.geometryId;
    target.mLevelId = source.levelId;
    target.mProducerId = source.producerId;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mDeletionTime = source.deletionTime;
    target.mModificationTime = source.modificationTime;
    target.mStatus = source.status;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void Converter::convert(T::GeometryInfoList& source,C::GeometryInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GeometryInfo *info = source.getGeometryInfoByIndex(t);
      C::GeometryInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GeometryInfoList& source,T::GeometryInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::GeometryInfo corbaObject = source[t];
      T::GeometryInfo *info = new T::GeometryInfo();
      convert(corbaObject,*info);
      target.addGeometryInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(T::EventInfo& source,C::EventInfo& target)
{
  try
  {
    target.eventId = source.mEventId;
    target.eventTime = (CORBA::ULong)source.mEventTime;
    target.serverTime = (CORBA::ULong)source.mServerTime;
    target.type = source.mType;
    target.id1 = source.mId1;
    target.id2 = source.mId2;
    target.id3 = source.mId3;
    target.flags = source.mFlags;
    target.eventData= CORBA::string_dup(source.mEventData.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::EventInfo& source,T::EventInfo& target)
{
  try
  {
    target.mEventId = source.eventId;
    target.mEventTime = C_UINT(source.eventTime);
    target.mServerTime = C_UINT(source.serverTime);
    target.mType = source.type;
    target.mId1 = source.id1;
    target.mId2 = source.id2;
    target.mId3 = source.id3;
    target.mFlags = source.flags;
    target.mEventData = source.eventData;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::EventInfoList& source,C::EventInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    uint t = 0;
    T::EventInfo *it = source.getFirstEvent();
    while (it != nullptr)
    {
      C::EventInfo corbaObject;
      convert(*it,corbaObject);
      target[t] = corbaObject;
      it = it->nextItem;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::EventInfoList& source,T::EventInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::EventInfo corbaObject = source[t];
      T::EventInfo *info = new T::EventInfo();
      convert(corbaObject,*info);
      target.addEventInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(string_vec& source,C::StringList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = CORBA::string_dup(source[t].c_str());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const C::StringList& source,string_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::set<std::string>& source,C::TimeStringList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = CORBA::string_dup(it->c_str());
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const C::TimeStringList& source,std::set<std::string>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::set<std::string>& source,C::StringList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = CORBA::string_dup(it->c_str());
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const C::StringList& source,std::set<std::string>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::vector<uint>& source,C::UInt32List& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::UInt32List& source,std::vector<uint>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::vector<UInt64>& source,C::FileIdList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::FileIdList& source,std::vector<UInt64>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.emplace_back(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::set<UInt64>& source,C::GenerationIdList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GenerationIdList& source,std::set<UInt64>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::set<UInt64>& source,C::FileIdList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::FileIdList& source,std::set<UInt64>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::set<int>& source,C::GeometryIdList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::GeometryIdList& source,std::set<int>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(std::set<int>& source,C::LevelList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);

    uint t = 0;
    for (auto it=source.begin(); it!=source.end(); ++it)
    {
      target[t] = *it;
      t++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::LevelList& source,std::set<int>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.insert(source[t]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(T::FileAndContent& source,C::FileContent& target)
{
  try
  {
    convert(source.mFileInfo,target.fileInfo);
    convert(source.mContentInfoList,target.contentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::FileContent& source,T::FileAndContent& target)
{
  try
  {
    convert(source.fileInfo,target.mFileInfo);
    convert(source.contentInfoList,target.mContentInfoList);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(std::vector<T::FileAndContent>& source,C::FileContentList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t = 0; t<len; t++)
    {
      C::FileContent corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const C::FileContentList& source,std::vector<T::FileAndContent>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      T::FileAndContent fc;
      convert(source[t],fc);
      target.emplace_back(fc);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::ForecastTime& source,SmartMet::C::ForecastTimeInfo& target)
{
  try
  {
    target.sourceId = source.mSourceId;
    target.generationId = source.mGenerationId;
    target.geometryId = source.mGeometryId;
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.forecastTime = CORBA::string_dup(source.mForecastTime.c_str());
    target.modificationTime = CORBA::string_dup(source.mModificationTime.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::C::ForecastTimeInfo& source,T::ForecastTime& target)
{
  try
  {
    target.mSourceId = source.sourceId;
    target.mGenerationId = source.generationId;
    target.mGeometryId = source.geometryId;
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mForecastTime = source.forecastTime;
    target.mModificationTime = source.modificationTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



void Converter::convert(std::vector<T::ForecastTime>& source,SmartMet::C::ForecastTimeInfoList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t = 0; t<len; t++)
    {
      C::ForecastTimeInfo corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::C::ForecastTimeInfoList& source,std::vector<T::ForecastTime>& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    target.reserve(len);
    for (uint t=0; t<len; t++)
    {
      T::ForecastTime ft;
      convert(source[t],ft);
      target.emplace_back(ft);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







void Converter::convert(T::LevelInfo& source,C::LevelInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.parameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.levelId = source.mFmiParameterLevelId;
    target.level = source.mParameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::LevelInfo& source,T::LevelInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mFmiParameterName = source.parameterName;
    target.mFmiParameterLevelId = source.levelId;
    target.mParameterLevel = source.level;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::LevelInfoList& source,C::LevelInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::LevelInfo *info = source.getLevelInfoByIndex(t);
      C::LevelInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const C::LevelInfoList& source,T::LevelInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      C::LevelInfo corbaObject = source[t];
      T::LevelInfo *info = new T::LevelInfo();
      convert(corbaObject,*info);
      target.addLevelInfo(info);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}







}
}
}


