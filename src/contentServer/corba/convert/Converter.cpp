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





void Converter::convert(T::ContentInfo& source,ContentServer::Corba::CorbaContentInfo& target)
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
    target.fmiParameterId = source.mFmiParameterId;
    target.fmiParameterName = CORBA::string_dup(source.getFmiParameterName());
    target.gribParameterId = source.mGribParameterId;
    target.newbaseParameterId = source.mNewbaseParameterId;
    target.newbaseParameterName = CORBA::string_dup(source.getNewbaseParameterName());
    target.netCdfParameterName = CORBA::string_dup(source.getNetCdfParameterName());
    target.fmiParameterLevelId = source.mFmiParameterLevelId;
    target.grib1ParameterLevelId = source.mGrib1ParameterLevelId;
    target.grib2ParameterLevelId = source.mGrib2ParameterLevelId;
    target.parameterLevel = source.mParameterLevel;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.geometryId = source.mGeometryId;
    target.modificationTime = source.mModificationTime;
    target.deletionTime = source.mDeletionTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaContentInfo& source,T::ContentInfo& target)
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
    target.mFmiParameterId = source.fmiParameterId;
    target.setFmiParameterName(source.fmiParameterName);
    target.mGribParameterId = source.gribParameterId;
    target.mNewbaseParameterId = source.newbaseParameterId;
    target.setNewbaseParameterName(source.newbaseParameterName);
    target.setNetCdfParameterName(source.netCdfParameterName);
    target.mFmiParameterLevelId = source.fmiParameterLevelId;
    target.mGrib1ParameterLevelId = source.grib1ParameterLevelId;
    target.mGrib2ParameterLevelId = source.grib2ParameterLevelId;
    target.mParameterLevel = source.parameterLevel;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mGeometryId = source.geometryId;
    target.mModificationTime = source.modificationTime;
    target.mDeletionTime = source.deletionTime;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::ContentInfoList& source,ContentServer::Corba::CorbaContentInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = source.getContentInfoByIndex(t);
      ContentServer::Corba::CorbaContentInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaContentInfoList& source,T::ContentInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaContentInfo corbaObject = source[t];
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






void Converter::convert(T::FileInfo& source,ContentServer::Corba::CorbaFileInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileId = source.mFileId;
    target.fileType = (CORBA::Octet)source.mFileType;
    target.name = CORBA::string_dup(source.mName.c_str());
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





void Converter::convert(const ContentServer::Corba::CorbaFileInfo& source,T::FileInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileId = source.fileId;
    target.mFileType = source.fileType;
    target.mName = source.name;
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





void Converter::convert(T::FileInfoList& source,ContentServer::Corba::CorbaFileInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::FileInfo *info = source.getFileInfoByIndex(t);
      ContentServer::Corba::CorbaFileInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaFileInfoList& source,T::FileInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaFileInfo corbaObject = source[t];
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





void Converter::convert(T::ProducerInfo& source,ContentServer::Corba::CorbaProducerInfo& target)
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





void Converter::convert(const ContentServer::Corba::CorbaProducerInfo& source,T::ProducerInfo& target)
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





void Converter::convert(T::ProducerInfoList& source,ContentServer::Corba::CorbaProducerInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *info = source.getProducerInfoByIndex(t);
      ContentServer::Corba::CorbaProducerInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaProducerInfoList& source,T::ProducerInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaProducerInfo corbaObject = source[t];
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




void Converter::convert(T::GenerationInfo& source,ContentServer::Corba::CorbaGenerationInfo& target)
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
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaGenerationInfo& source,T::GenerationInfo& target)
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
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::GenerationInfoList& source,ContentServer::Corba::CorbaGenerationInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *info = source.getGenerationInfoByIndex(t);
      ContentServer::Corba::CorbaGenerationInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaGenerationInfoList& source,T::GenerationInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaGenerationInfo corbaObject = source[t];
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





void Converter::convert(T::EventInfo& source,ContentServer::Corba::CorbaEventInfo& target)
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
    target.note= CORBA::string_dup(source.mNote.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaEventInfo& source,T::EventInfo& target)
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
    target.mNote = source.note;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::EventInfoList& source,ContentServer::Corba::CorbaEventInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    uint t = 0;
    T::EventInfo *it = source.getFirstEvent();
    while (it != nullptr)
    {
      ContentServer::Corba::CorbaEventInfo corbaObject;
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





void Converter::convert(const ContentServer::Corba::CorbaEventInfoList& source,T::EventInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaEventInfo corbaObject = source[t];
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





void Converter::convert(string_vec& source,ContentServer::Corba::CorbaStringList& target)
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




void Converter::convert(const ContentServer::Corba::CorbaStringList& source,string_vec& target)
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





void Converter::convert(std::set<std::string>& source,ContentServer::Corba::CorbaStringList& target)
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




void Converter::convert(const ContentServer::Corba::CorbaStringList& source,std::set<std::string>& target)
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





void Converter::convert(std::vector<uint>& source,ContentServer::Corba::CorbaULongList& target)
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





void Converter::convert(const ContentServer::Corba::CorbaULongList& source,std::vector<uint>& target)
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





void Converter::convert(std::set<uint>& source,ContentServer::Corba::CorbaULongList& target)
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





void Converter::convert(const ContentServer::Corba::CorbaULongList& source,std::set<uint>& target)
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





void Converter::convert(std::set<int>& source,ContentServer::Corba::CorbaLongList& target)
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





void Converter::convert(const ContentServer::Corba::CorbaLongList& source,std::set<int>& target)
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




void Converter::convert(T::FileAndContent& source,ContentServer::Corba::CorbaFileContent& target)
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





void Converter::convert(const ContentServer::Corba::CorbaFileContent& source,T::FileAndContent& target)
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





void Converter::convert(std::vector<T::FileAndContent>& source,ContentServer::Corba::CorbaFileContentList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t = 0; t<len; t++)
    {
      ContentServer::Corba::CorbaFileContent corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void Converter::convert(const ContentServer::Corba::CorbaFileContentList& source,std::vector<T::FileAndContent>& target)
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





void Converter::convert(T::ForecastTime& source,SmartMet::ContentServer::Corba::CorbaForecastTime& target)
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





void Converter::convert(const SmartMet::ContentServer::Corba::CorbaForecastTime& source,T::ForecastTime& target)
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



void Converter::convert(std::vector<T::ForecastTime>& source,SmartMet::ContentServer::Corba::CorbaForecastTimeList& target)
{
  try
  {
    uint len = source.size();
    target.length(len);
    for (uint t = 0; t<len; t++)
    {
      ContentServer::Corba::CorbaForecastTime corbaObject;
      convert(source[t],corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const SmartMet::ContentServer::Corba::CorbaForecastTimeList& source,std::vector<T::ForecastTime>& target)
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







void Converter::convert(T::LevelInfo& source,ContentServer::Corba::CorbaLevelInfo& target)
{
  try
  {
    target.producerId = source.mProducerId;
    target.fmiParameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.fmiParameterLevelId = source.mFmiParameterLevelId;
    target.grib1ParameterLevelId = source.mGrib1ParameterLevelId;
    target.grib2ParameterLevelId = source.mGrib2ParameterLevelId;
    target.parameterLevel = source.mParameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaLevelInfo& source,T::LevelInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mFmiParameterName = source.fmiParameterName;
    target.mFmiParameterLevelId = source.fmiParameterLevelId;
    target.mGrib1ParameterLevelId = source.grib1ParameterLevelId;
    target.mGrib2ParameterLevelId = source.grib2ParameterLevelId;
    target.mParameterLevel = source.parameterLevel;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(T::LevelInfoList& source,ContentServer::Corba::CorbaLevelInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::LevelInfo *info = source.getLevelInfoByIndex(t);
      ContentServer::Corba::CorbaLevelInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaLevelInfoList& source,T::LevelInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaLevelInfo corbaObject = source[t];
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


