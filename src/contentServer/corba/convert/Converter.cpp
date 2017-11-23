#include "Converter.h"
#include <grid-files/common/Exception.h>

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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





Converter::~Converter()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(T::ContentInfo& source,ContentServer::Corba::CorbaContentInfo& target)
{
  try
  {
    target.serverFlags = source.mServerFlags;
    target.groupFlags = source.mGroupFlags;
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileType = (CORBA::Octet)source.mFileType;
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.forecastTime = CORBA::string_dup(source.mForecastTime.c_str());
    target.forecastType = source.mForecastType;
    target.forecastNumber = source.mForecastNumber;
    target.fmiParameterId = CORBA::string_dup(source.mFmiParameterId.c_str());
    target.fmiParameterName = CORBA::string_dup(source.mFmiParameterName.c_str());
    target.gribParameterId = CORBA::string_dup(source.mGribParameterId.c_str());
    target.cdmParameterId = CORBA::string_dup(source.mCdmParameterId.c_str());
    target.cdmParameterName = CORBA::string_dup(source.mCdmParameterName.c_str());
    target.newbaseParameterId = CORBA::string_dup(source.mNewbaseParameterId.c_str());
    target.newbaseParameterName = CORBA::string_dup(source.mNewbaseParameterName.c_str());
    target.fmiParameterLevelId = source.mFmiParameterLevelId;
    target.grib1ParameterLevelId = source.mGrib1ParameterLevelId;
    target.grib2ParameterLevelId = source.mGrib2ParameterLevelId;
    target.parameterLevel = source.mParameterLevel;
    target.fmiParameterUnits = CORBA::string_dup(source.mFmiParameterUnits.c_str());
    target.gribParameterUnits = CORBA::string_dup(source.mGribParameterUnits.c_str());
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
    target.geometryId = source.mGeometryId;
    target.modificationTime = CORBA::string_dup(source.mModificationTime.c_str());
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(const ContentServer::Corba::CorbaContentInfo& source,T::ContentInfo& target)
{
  try
  {
    target.mServerFlags = source.serverFlags;
    target.mGroupFlags = source.groupFlags;
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileType = (T::FileType)source.fileType;
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mForecastTime = source.forecastTime;
    target.mForecastType = source.forecastType;
    target.mForecastNumber = source.forecastNumber;
    target.mFmiParameterId = source.fmiParameterId;
    target.mFmiParameterName = source.fmiParameterName;
    target.mGribParameterId = source.gribParameterId;
    target.mCdmParameterId = source.cdmParameterId;
    target.mCdmParameterName = source.cdmParameterName;
    target.mNewbaseParameterId = source.newbaseParameterId;
    target.mNewbaseParameterName = source.newbaseParameterName;
    target.mFmiParameterLevelId = source.fmiParameterLevelId;
    target.mGrib1ParameterLevelId = source.grib1ParameterLevelId;
    target.mGrib2ParameterLevelId = source.grib2ParameterLevelId;
    target.mParameterLevel = source.parameterLevel;
    target.mFmiParameterUnits = source.fmiParameterUnits;
    target.mGribParameterUnits = source.gribParameterUnits;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
    target.mGeometryId = source.geometryId;
    target.mModificationTime = source.modificationTime;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}






void Converter::convert(T::FileInfo& source,ContentServer::Corba::CorbaFileInfo& target)
{
  try
  {
    target.groupFlags = source.mGroupFlags;
    target.producerId = source.mProducerId;
    target.generationId = source.mGenerationId;
    target.fileId = source.mFileId;
    target.fileType = (CORBA::Octet)source.mFileType;
    target.name = CORBA::string_dup(source.mName.c_str());
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaFileInfo& source,T::FileInfo& target)
{
  try
  {
    target.mGroupFlags = source.groupFlags;
    target.mProducerId = source.producerId;
    target.mGenerationId = source.generationId;
    target.mFileId = source.fileId;
    target.mFileType = (T::FileType)source.fileType;
    target.mName = source.name;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaFileInfoList& source,T::FileInfoList& target)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(T::ServerInfo& source,ContentServer::Corba::CorbaServerInfo& target)
{
  try
  {
    target.serverId = source.mServerId;
    target.name = CORBA::string_dup(source.mName.c_str());
    target.serverIor = CORBA::string_dup(source.mServerIor.c_str());
    target.type = source.mType;
    target.flags = source.mFlags;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaServerInfo& source,T::ServerInfo& target)
{
  try
  {
    target.mServerId = source.serverId;
    target.mName = source.name;
    target.mServerIor = source.serverIor;
    target.mType = source.type;
    target.mFlags = source.flags;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(T::ServerInfoList& source,ContentServer::Corba::CorbaServerInfoList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::ServerInfo *info = source.getServerInfoByIndex(t);
      ContentServer::Corba::CorbaServerInfo corbaObject;
      convert(*info,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void Converter::convert(ContentServer::Corba::CorbaServerInfoList& source,T::ServerInfoList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      ContentServer::Corba::CorbaServerInfo corbaObject = source[t];
      T::ServerInfo *info = new T::ServerInfo();
      convert(corbaObject,*info);
      target.addServerInfo(info);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaProducerInfo& source,T::ProducerInfo& target)
{
  try
  {
    target.mProducerId = source.producerId;
    target.mName = source.name;
    target.mTitle = source.title;
    target.mDescription = source.description;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaProducerInfoList& source,T::ProducerInfoList& target)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    target.status = (CORBA::Octet)source.mStatus;
    target.flags = source.mFlags;
    target.sourceId = source.mSourceId;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaGenerationInfo& source,T::GenerationInfo& target)
{
  try
  {
    target.mGenerationId = source.generationId;
    target.mGenerationType = source.generationType;
    target.mProducerId = source.producerId;
    target.mName = source.name;
    target.mDescription = source.description;
    target.mStatus = (T::GenerationStatus)source.status;
    target.mFlags = source.flags;
    target.mSourceId = source.sourceId;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaGenerationInfoList& source,T::GenerationInfoList& target)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(T::EventInfo& source,ContentServer::Corba::CorbaEventInfo& target)
{
  try
  {
    target.eventId = source.mEventId;
    target.eventTime = (CORBA::ULong)source.mEventTime;
    target.serverTime = (CORBA::ULong)source.mServerTime;
    target.type = (unsigned int)source.mType;
    target.id1 = source.mId1;
    target.id2 = source.mId2;
    target.id3 = source.mId3;
    target.flags = source.mFlags;
    target.note= CORBA::string_dup(source.mNote.c_str());
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaEventInfo& source,T::EventInfo& target)
{
  try
  {
    target.mEventId = source.eventId;
    target.mEventTime = (uint)source.eventTime;
    target.mServerTime = (uint)source.serverTime;
    target.mType = (ContentServer::EventType)source.type;
    target.mId1 = source.id1;
    target.mId2 = source.id2;
    target.mId3 = source.id3;
    target.mFlags = source.flags;
    target.mNote = source.note;
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    while (it != NULL)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(ContentServer::Corba::CorbaEventInfoList& source,T::EventInfoList& target)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(string_vec& source,ContentServer::Corba::CorbaStringList& target)
{
  try
  {
    uint len = (uint)source.size();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      target[t] = CORBA::string_dup(source[t].c_str());
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void Converter::convert(ContentServer::Corba::CorbaStringList& source,string_vec& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      target.push_back(std::string(source[t]));
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(std::set<std::string>& source,ContentServer::Corba::CorbaStringList& target)
{
  try
  {
    uint len = (uint)source.size();
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void Converter::convert(ContentServer::Corba::CorbaStringList& source,std::set<std::string>& target)
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(std::set<uint>& source,ContentServer::Corba::CorbaULongList& target)
{
  try
  {
    uint len = (uint)source.size();
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void Converter::convert(std::set<int>& source,ContentServer::Corba::CorbaLongList& target)
{
  try
  {
    uint len = (uint)source.size();
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}
}
}
}


