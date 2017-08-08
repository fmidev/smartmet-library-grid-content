#include "ClientImplementation.h"
#include "grid-files/common/Exception.h"
#include <curl/curl.h>



namespace SmartMet
{
namespace ContentServer
{
namespace HTTP
{


ClientImplementation::ClientImplementation()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ClientImplementation::~ClientImplementation()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ClientImplementation::init(std::string url)
{
  try
  {
    mUrl = url;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_clear(T::SessionId sessionId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","clear");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_reload(T::SessionId sessionId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","reload");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addDataServerInfo(T::SessionId sessionId,T::ServerInfo& serverInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addDataServerInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("serverInfo",serverInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      serverInfo.setCsv(response.getLineByKey("serverInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteDataServerInfoById(T::SessionId sessionId,uint serverId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteDataServerInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getDataServerInfoById(T::SessionId sessionId,uint serverId,T::ServerInfo& serverInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getDataServerInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      serverInfo.setCsv(response.getLineByIndex(1));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getDataServerInfoByName(T::SessionId sessionId,std::string serverName,T::ServerInfo& serverInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getDataServerInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("serverName",serverName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      serverInfo.setCsv(response.getLineByKey("serverInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getDataServerInfoByIor(T::SessionId sessionId,std::string serverIor,T::ServerInfo& serverInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getDataServerInfoByIor");
    request.addLine("sessionId",sessionId);
    request.addLine("serverIor",serverIor);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      serverInfo.setCsv(response.getLineByKey("serverInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getDataServerInfoList(T::SessionId sessionId,T::ServerInfoList& serverInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getDataServerInfoList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("serverInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ServerInfo *info = new T::ServerInfo();
        info->setCsv(lines[t]);
        serverInfoList.addServerInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getDataServerInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getDataServerInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addProducerInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("producerInfo",producerInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteProducerInfoById(T::SessionId sessionId,uint producerId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteProducerInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteProducerInfoByName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteProducerInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteProducerInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getProducerInfoById(T::SessionId sessionId,uint producerId,T::ProducerInfo& producerInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getProducerInfoByName(T::SessionId sessionId,std::string producerName,T::ProducerInfo& producerInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getProducerInfoList(T::SessionId sessionId,T::ProducerInfoList& producerInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("producerInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ProducerInfo *info = new T::ProducerInfo();
        info->setCsv(lines[t]);
        producerInfoList.addProducerInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getProducerInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::ProducerInfoList& producerInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("producerInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ProducerInfo *info = new T::ProducerInfo();
        info->setCsv(lines[t]);
        producerInfoList.addProducerInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getProducerInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addGenerationInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("generationInfo",generationInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoById(T::SessionId sessionId,uint generationId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoById(T::SessionId sessionId,uint generationId,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoByName(T::SessionId sessionId,std::string generationName,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoList(T::SessionId sessionId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("generationInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = new T::GenerationInfo();
        info->setCsv(lines[t]);
        generationInfoList.addGenerationInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("generationInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = new T::GenerationInfo();
        info->setCsv(lines[t]);
        generationInfoList.addGenerationInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,std::string producerName,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("generationInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = new T::GenerationInfo();
        info->setCsv(lines[t]);
        generationInfoList.addGenerationInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("generationInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = new T::GenerationInfo();
        info->setCsv(lines[t]);
        generationInfoList.addGenerationInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLastGenerationInfoByProducerIdAndStatus");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("status",(uint)generationStatus);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,std::string producerName,T::GenerationStatus generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLastGenerationInfoByProducerNameAndStatus");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("status",(uint)generationStatus);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getGenerationInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,T::GenerationStatus status)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGenerationInfoStatusById");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("status",(uint)status);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,std::string generationName,T::GenerationStatus status)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGenerationInfoStatusByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("status",(uint)status);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addFileInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("fileInfo",fileInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addFileInfoWithContentList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoById(T::SessionId sessionId,uint fileId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoByName(T::SessionId sessionId,std::string filename)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByGroupFlags");
    request.addLine("sessionId",sessionId);
    request.addLine("groupFlags",groupFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

   T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteFileInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoById(T::SessionId sessionId,uint fileId,T::FileInfo& fileInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoByName(T::SessionId sessionId,std::string filename,T::FileInfo& fileInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoList");
    request.addLine("sessionId",sessionId);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByGroupFlags");
    request.addLine("sessionId",sessionId);
    request.addLine("groupFlags",groupFlags);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint maxRecords,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);
    request.addLine("startFileId",startFileId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = new T::FileInfo();
        info->setCsv(lines[t]);
        fileInfoList.addFileInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addEventInfo(T::SessionId sessionId,T::EventInfo& eventInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addEventInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("eventInfo",eventInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      eventInfo.setCsv(response.getLineByKey("eventInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getLastEventInfo(T::SessionId sessionId,uint requestingServerId,T::EventInfo& eventInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLastEventInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("requestingServerId",requestingServerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      eventInfo.setCsv(response.getLineByKey("eventInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getEventInfoList");
    request.addLine("sessionId",sessionId);
    request.addLine("requestingServerId",requestingServerId);
    request.addLine("startEventId",startEventId);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("fileInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::EventInfo *info = new T::EventInfo();
        info->setCsv(lines[t]);
        eventInfoList.addEventInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getEventInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getEventInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addContent");
    request.addLine("sessionId",sessionId);
    request.addLine("contentInfo",contentInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      contentInfo.setCsv(response.getLineByKey("contentInfo"));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_addContentList(T::SessionId sessionId,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addContentList");
    request.addLine("sessionId",sessionId);
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      request.addLine("contentInfo",info->getCsv());
    }

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);
    request.addLine("messageIndex",messageIndex);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByFileId(T::SessionId sessionId,uint fileId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByFileName(T::SessionId sessionId,std::string filename)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByGroupFlags(T::SessionId sessionId,uint groupFlags)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByGroupFlags");
    request.addLine("sessionId",sessionId);
    request.addLine("groupFlags",groupFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByProducerName(T::SessionId sessionId,std::string producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByGenerationId(T::SessionId sessionId,uint generationId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,std::string generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_deleteContentListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_registerContentList(T::SessionId sessionId,uint serverId,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","registerContentList");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_registerContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","registerContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_unregisterContentList(T::SessionId sessionId,uint serverId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","unregisterContentList");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_unregisterContentListByFileId(T::SessionId sessionId,uint serverId,uint fileId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","unregisterContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",serverId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentInfo(T::SessionId sessionId,uint fileId,uint messageIndex,T::ContentInfo& contentInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);
    request.addLine("messageIndex",messageIndex);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      contentInfo.setCsv(response.getLineByIndex(1));

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentList");
    request.addLine("sessionId",sessionId);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByFileId(T::SessionId sessionId,uint fileId,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("fileId",fileId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByFileName(T::SessionId sessionId,std::string filename,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByFileName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByGroupFlags(T::SessionId sessionId,uint groupFlags,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByGroupFlags");
    request.addLine("sessionId",sessionId);
    request.addLine("groupFlags",groupFlags);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByProducerName(T::SessionId sessionId,std::string producerName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByServerId(T::SessionId sessionId,uint serverId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByServerId");
    request.addLine("sessionId",sessionId);
    request.addLine("serverId",sessionId);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByGenerationName(T::SessionId sessionId,std::string generationName,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByGenerationIdAndTimeRange");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,std::string generationName,std::string startTime,std::string endTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByGenerationNameAndTimeRange");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,uint maxRecords,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);
    request.addLine("startFileId",startFileId);
    request.addLine("startMessageIndex",startMessageIndex);
    request.addLine("maxRecords",maxRecords);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameter");
    request.addLine("sessionId",sessionId);
    request.addLine("parameterKeyType",(uint)parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelIdType",(uint)parameterLevelIdType);
    request.addLine("parameterLevelId",(uint)parameterLevelId);
    request.addLine("minLevel",(int)minLevel);
    request.addLine("maxLevel",(int)maxLevel);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("parameterKeyType",(uint)parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelIdType",(uint)parameterLevelIdType);
    request.addLine("parameterLevelId",(uint)parameterLevelId);
    request.addLine("minLevel",(int)minLevel);
    request.addLine("maxLevel",(int)maxLevel);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,std::string generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("parameterKeyType",(uint)parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelIdType",(uint)parameterLevelIdType);
    request.addLine("parameterLevelId",(uint)parameterLevelId);
    request.addLine("minLevel",(int)minLevel);
    request.addLine("maxLevel",(int)maxLevel);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



int ClientImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("parameterKeyType",(uint)parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelIdType",(uint)parameterLevelIdType);
    request.addLine("parameterLevelId",(uint)parameterLevelId);
    request.addLine("minLevel",(int)minLevel);
    request.addLine("maxLevel",(int)maxLevel);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




int ClientImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,std::string producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelIdType parameterLevelIdType,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,std::string startTime,std::string endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("parameterKeyType",(uint)parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelIdType",(uint)parameterLevelIdType);
    request.addLine("parameterLevelId",(uint)parameterLevelId);
    request.addLine("minLevel",(int)minLevel);
    request.addLine("maxLevel",(int)maxLevel);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      std::vector<std::string> lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentInfoList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





size_t ClientImplementation_responseProcessing(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  try
  {
    T::ResponseMessage *response = (T::ResponseMessage*)userdata;

    char line[10000];
    uint c = 0;
    for (size_t t=0; t<nmemb; t++)
    {
      char ch = ptr[t];
      if (ch == '\r'  || ch == '\n')
      {
        line[c] = '\0';
        if (c > 0)
          response->addLine(line);

        c = 0;
      }
      else
      {
        line[c] = ch;
        c++;
      }
    }

    if (c > 0)
    {
      line[c] = '\0';
      response->addLine(line);
    }

    return nmemb;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}






int ClientImplementation::_getContentCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = (int)response.getLineValueByKey("result");
    if (result == Result::OK)
      count = (uint)response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ClientImplementation::sendRequest(T::RequestMessage& request,T::ResponseMessage& response)
{
  try
  {
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl)
    {
      curl_easy_setopt(curl,CURLOPT_PROXY,"");
      curl_easy_setopt(curl, CURLOPT_URL,mUrl.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ClientImplementation_responseProcessing);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

      char *data = new char[request.getContentSize() + 100];
      char *p = data;
      uint lineCount = request.getLineCount();

      for (uint t=0; t<lineCount; t++)
      {
        std::string s = request.getLineByIndex(t);
        p += sprintf(p,"%s\n",s.c_str());
      }
      printf("%s\n",data);

      curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);

      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

      delete data;
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    //request.print(std::cout,0,0);

    //response.print(std::cout,0,0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}
}
