#include "ClientImplementation.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>



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
    mImplementationType = Implementation::HttpClient;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ClientImplementation::~ClientImplementation()
{
  try
  {
    if (curl != nullptr)
      curl_easy_cleanup(curl);

    curl_global_cleanup();
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteProducerInfoByName(T::SessionId sessionId,const std::string& producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteProducerInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getProducerInfoByName(T::SessionId sessionId,const std::string& producerName,T::ProducerInfo& producerInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      producerInfo.setCsv(response.getLineByKey("producerInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getProducerInfoListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ProducerInfoList& producerInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerInfoListByParameter");
    request.addLine("sessionId",sessionId);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getProducerNameAndGeometryList(T::SessionId sessionId,std::set<std::string>& list)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerNameAndGeometryList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("line",lines);
      for (uint t=0; t<len; t++)
      {
        list.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getProducerParameterList(T::SessionId sessionId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerParameterList");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceParameterKeyType",sourceParameterKeyType);
    request.addLine("targetParameterKeyType",targetParameterKeyType);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("line",lines);
      for (uint t=0; t<len; t++)
      {
        list.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getProducerParameterListByProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType sourceParameterKeyType,T::ParamKeyType targetParameterKeyType,std::set<std::string>& list)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getProducerParameterListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("sourceParameterKeyType",sourceParameterKeyType);
    request.addLine("targetParameterKeyType",targetParameterKeyType);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("line",lines);
      for (uint t=0; t<len; t++)
      {
        list.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setProducerInfo(T::SessionId sessionId,T::ProducerInfo& producerInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setProducerInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("producerInfo",producerInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteGenerationInfoByName(T::SessionId sessionId,const std::string& generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteGenerationInfoListByIdList(T::SessionId sessionId,std::set<uint>& generationIdList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoListByIdList");
    request.addLine("sessionId",sessionId);

    for (auto it=generationIdList.begin(); it!=generationIdList.end(); ++it)
      request.addLine("generationId",*it);


    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGenerationInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getGenerationIdGeometryIdAndForecastTimeList(T::SessionId sessionId,std::set<std::string>& list)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationIdGeometryIdAndForecastTimeList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("line",lines);
      for (uint t=0; t<len; t++)
      {
        list.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getGenerationInfoByName(T::SessionId sessionId,const std::string& generationName,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getGenerationInfoListByGeometryId(T::SessionId sessionId,T::GeometryId geometryId,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoListByGeometryId");
    request.addLine("sessionId",sessionId);
    request.addLine("geometryId",geometryId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getGenerationInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,T::GenerationInfoList& generationInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGenerationInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerIdAndStatus(T::SessionId sessionId,uint producerId,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLastGenerationInfoByProducerIdAndStatus");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("status",generationStatus);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getLastGenerationInfoByProducerNameAndStatus(T::SessionId sessionId,const std::string& producerName,uchar generationStatus,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLastGenerationInfoByProducerNameAndStatus");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("status",generationStatus);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      generationInfo.setCsv(response.getLineByKey("generationInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setGenerationInfo(T::SessionId sessionId,T::GenerationInfo& generationInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGenerationInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("generationInfo",generationInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setGenerationInfoStatusById(T::SessionId sessionId,uint generationId,uchar status)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGenerationInfoStatusById");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("status",status);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setGenerationInfoStatusByName(T::SessionId sessionId,const std::string& generationName,uchar status)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGenerationInfoStatusByName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("status",status);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_addGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addGeometryInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("geometryInfo",geometryInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      geometryInfo.setCsv(response.getLineByKey("geometryInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGeometryInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("geometryId",geometryId);
    request.addLine("levelId",levelId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_deleteGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGeometryInfoListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_deleteGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGeometryInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_deleteGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteGeometryInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,T::GeometryInfo& geometryInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoById");
    request.addLine("sessionId",sessionId);
    request.addLine("geometryId",geometryId);
    request.addLine("levelId",levelId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      geometryInfo.setCsv(response.getLineByKey("geometryInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoList(T::SessionId sessionId,T::GeometryInfoList& geometryInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("geometryInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GeometryInfo *info = new T::GeometryInfo();
        info->setCsv(lines[t]);
        geometryInfoList.addGeometryInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoListByGenerationId(T::SessionId sessionId,uint generationId,T::GeometryInfoList& geometryInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("geometryInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GeometryInfo *info = new T::GeometryInfo();
        info->setCsv(lines[t]);
        geometryInfoList.addGeometryInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoListByProducerId(T::SessionId sessionId,uint producerId,T::GeometryInfoList& geometryInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("geometryInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GeometryInfo *info = new T::GeometryInfo();
        info->setCsv(lines[t]);
        geometryInfoList.addGeometryInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoListBySourceId(T::SessionId sessionId,uint sourceId,T::GeometryInfoList& geometryInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoListBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("geometryInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::GeometryInfo *info = new T::GeometryInfo();
        info->setCsv(lines[t]);
        geometryInfoList.addGeometryInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getGeometryInfoCount(T::SessionId sessionId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getGeometryInfoCount");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_setGeometryInfo(T::SessionId sessionId,T::GeometryInfo& geometryInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGeometryInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("geometryInfo",geometryInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_setGeometryInfoStatusById(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamLevelId levelId,uchar status)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setGeometryInfoStatusById");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("geometryId",geometryId);
    request.addLine("levelId",levelId);
    request.addLine("status",status);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_addFileInfoWithContentList(T::SessionId sessionId,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","addFileInfoWithContentList");
    request.addLine("sessionId",sessionId);
    request.addLine("fileInfo",fileInfo.getCsv());

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
      request.addLine("contentInfo",info->getCsv());
    }

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_addFileInfoListWithContent(T::SessionId sessionId,uint requestFlags,std::vector<T::FileAndContent>& fileAndContentList)
{
  try
  {
    T::RequestMessage request;

    uint length = fileAndContentList.size();

    request.addLine("method","addFileInfoListWithContent");
    request.addLine("sessionId",sessionId);
    request.addLine("requestFlags",requestFlags);
    request.addLine("length",length);

    char tmp[100];
    for (uint f=0; f<length; f++)
    {
      sprintf(tmp,"fileInfo.%u",f);
      request.addLine(tmp,fileAndContentList[f].mFileInfo.getCsv());

      sprintf(tmp,"contentInfo.%u",f);

      uint len = fileAndContentList[f].mContentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = fileAndContentList[f].mContentInfoList.getContentInfoByIndex(t);
        request.addLine(tmp,info->getCsv());
      }
    }

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      char tmp[100];
      for (uint f=0; f<length; f++)
      {
        sprintf(tmp,"fileInfo.%u",f);
        fileAndContentList[f].mFileInfo.setCsv(response.getLineByKey(tmp));

        sprintf(tmp,"contentInfo.%u",f);
        string_vec lines;
        uint len = response.getLinesByKey(tmp,lines);
        for (uint t=0; t<len; t++)
        {
          T::ContentInfo *info = fileAndContentList[f].mContentInfoList.getContentInfoByIndex(t);
          if (info!= nullptr)
            info->setCsv(lines[t]);
        }
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoByName(T::SessionId sessionId,const std::string& filename)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoListByForecastTimeList(T::SessionId sessionId,std::vector<T::ForecastTime>& forecastTimeList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByForecastTimeList");
    request.addLine("sessionId",sessionId);

    for (auto it = forecastTimeList.begin();it != forecastTimeList.end();++it)
    {
      request.addLine("forecastTime",it->getCsv());
    }

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ForecastType forecastType,T::ForecastNumber forecastNumber,const std::string& forecastTime)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByGenerationIdAndForecastTime");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("geometryId",geometryId);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("forecastTime",forecastTime);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteFileInfoListByFileIdList(T::SessionId sessionId,std::set<uint>& fileIdList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteFileInfoListByFileIdList");
    request.addLine("sessionId",sessionId);

    for (auto it=fileIdList.begin(); it!=fileIdList.end(); ++it)
      request.addLine("fileId",*it);


    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
 }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoByName(T::SessionId sessionId,const std::string& filename,T::FileInfo& fileInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoByName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      fileInfo.setCsv(response.getLineByKey("fileInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoList(T::SessionId sessionId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::FileInfoList& fileInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoListByFileIdList");
    request.addLine("sessionId",sessionId);

    for (auto it=fileIdList.begin(); it!=fileIdList.end(); ++it)
      request.addLine("fileId",*it);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ClientImplementation::_getFileInfoListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoCountByProducerId(T::SessionId sessionId,uint producerId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoCountByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoCountByGenerationId(T::SessionId sessionId,uint generationId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoCountByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoCountBySourceId(T::SessionId sessionId,uint sourceId,uint& count)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getFileInfoCountBySourceId");
    request.addLine("sessionId",sessionId);
    request.addLine("sourceId",sourceId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getFileInfoListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,int maxRecords,T::FileInfoList& fileInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setFileInfo(T::SessionId sessionId,T::FileInfo& fileInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setFileInfo");
    request.addLine("sessionId",sessionId);
    request.addLine("fileInfo",fileInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      eventInfo.setCsv(response.getLineByKey("eventInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      eventInfo.setCsv(response.getLineByKey("eventInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getEventInfoList(T::SessionId sessionId,uint requestingServerId,T::EventId startEventId,int maxRecords,T::EventInfoList& eventInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      contentInfo.setCsv(response.getLineByKey("contentInfo"));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteContentListByFileName(T::SessionId sessionId,const std::string& filename)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByFileId");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteContentListByProducerName(T::SessionId sessionId,const std::string& producerName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_deleteContentListByGenerationName(T::SessionId sessionId,const std::string& generationName)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteContentListByGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      contentInfo.setCsv(response.getLineByIndex(1));

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentList(T::SessionId sessionId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByFileIdList(T::SessionId sessionId,std::vector<uint>& fileIdList,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByFileIdList");
    request.addLine("sessionId",sessionId);

    for (auto it = fileIdList.begin(); it != fileIdList.end(); ++it)
      request.addLine("fileId",*it);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByFileName(T::SessionId sessionId,const std::string& filename,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByFileName");
    request.addLine("sessionId",sessionId);
    request.addLine("filename",filename);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByProducerId(T::SessionId sessionId,uint producerId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByProducerName(T::SessionId sessionId,const std::string& producerName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByGenerationId(T::SessionId sessionId,uint generationId,uint startFileId,uint startMessageIndex,int maxRecords,uint requestFlags,T::ContentInfoList& contentInfoList)
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
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByGenerationName(T::SessionId sessionId,const std::string& generationName,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByGenerationIdAndTimeRange(T::SessionId sessionId,uint generationId,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByGenerationNameAndTimeRange(T::SessionId sessionId,const std::string& generationName,const std::string& startTime,const std::string& endTime,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListBySourceId(T::SessionId sessionId,uint sourceId,uint startFileId,uint startMessageIndex,int maxRecords,T::ContentInfoList& contentInfoList)
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByParameter(T::SessionId sessionId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameter");
    request.addLine("sessionId",sessionId);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("minLevel",minLevel);
    request.addLine("maxLevel",maxLevel);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("minLevel",minLevel);
    request.addLine("maxLevel",maxLevel);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByParameterAndGenerationName(T::SessionId sessionId,const std::string& generationName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndGenerationName");
    request.addLine("sessionId",sessionId);
    request.addLine("generationName",generationName);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("minLevel",minLevel);
    request.addLine("maxLevel",maxLevel);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ClientImplementation::_getContentListByParameterAndProducerId(T::SessionId sessionId,uint producerId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("minLevel",minLevel);
    request.addLine("maxLevel",maxLevel);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




int ClientImplementation::_getContentListByParameterAndProducerName(T::SessionId sessionId,const std::string& producerName,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel minLevel,T::ParamLevel maxLevel,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& startTime,const std::string& endTime,uint requestFlags,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterAndProducerName");
    request.addLine("sessionId",sessionId);
    request.addLine("producerName",producerName);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("minLevel",minLevel);
    request.addLine("maxLevel",maxLevel);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("startTime",startTime);
    request.addLine("endTime",endTime);
    request.addLine("requestFlags",requestFlags);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentListByParameterGenerationIdAndForecastTime(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::string parameterKey,T::ParamLevelId parameterLevelId,T::ParamLevel level,T::ForecastType forecastType,T::ForecastNumber forecastNumber,T::GeometryId geometryId,const std::string& forecastTime,T::ContentInfoList& contentInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentListByParameterGenerationIdAndForecastTime");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("parameterKeyType",parameterKeyType);
    request.addLine("parameterKey",parameterKey);
    request.addLine("parameterLevelId",parameterLevelId);
    request.addLine("level",level);
    request.addLine("forecastType",forecastType);
    request.addLine("forecastNumber",forecastNumber);
    request.addLine("geometryId",geometryId);
    request.addLine("forecastTime",forecastTime);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


int ClientImplementation::_getContentGeometryIdListByGenerationId(T::SessionId sessionId,uint generationId,std::set<T::GeometryId>& geometryIdList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentGeometryIdListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("geometryId",lines);
      for (uint t=0; t<len; t++)
      {
        geometryIdList.insert(toInt64(lines[t].c_str()));
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentParamListByGenerationId(T::SessionId sessionId,uint generationId,T::ContentInfoList& contentParamList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentParamListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("contentInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo();
        info->setCsv(lines[t]);
        contentParamList.addContentInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentParamKeyListByGenerationId(T::SessionId sessionId,uint generationId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentParamKeyListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("parameterKeyType",(unsigned char)parameterKeyType);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("paramKey",lines);
      for (uint t=0; t<len; t++)
      {
        paramKeyList.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentParamKeyListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,T::ParamKeyType parameterKeyType,std::set<std::string>& paramKeyList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentParamKeyListByGenerationAndGeometryId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("geometryId",geometryId);
    request.addLine("parameterKeyType",(unsigned char)parameterKeyType);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("paramKey",lines);
      for (uint t=0; t<len; t++)
      {
        paramKeyList.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentTimeListByGenerationAndGeometryId(T::SessionId sessionId,uint generationId,T::GeometryId geometryId,std::set<std::string>& contentTimeList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentTimeListByGenerationAndGeometryId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);
    request.addLine("geometryId",geometryId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("contentTime",lines);
      for (uint t=0; t<len; t++)
      {
        contentTimeList.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentTimeListByGenerationId(T::SessionId sessionId,uint generationId,std::set<std::string>& contentTimeList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentTimeListByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("contentTime",lines);
      for (uint t=0; t<len; t++)
      {
        contentTimeList.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentTimeRangeByProducerAndGenerationId(T::SessionId sessionId,uint producerId,uint generationId,time_t& startTime,time_t& endTime)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentTimeRangeByProducerAndGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      startTime = (time_t)response.getLineInt64ValueByKey("startTime");
      endTime = (time_t)response.getLineInt64ValueByKey("endTime");
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getContentTimeRangeByGenerationId(T::SessionId sessionId,uint generationId,time_t& startTime,time_t& endTime)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentTimeRangeByGenerationId");
    request.addLine("sessionId",sessionId);
    request.addLine("generationId",generationId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      startTime = (time_t)response.getLineInt64ValueByKey("startTime");
      endTime = (time_t)response.getLineInt64ValueByKey("endTime");
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ClientImplementation::_getContentTimeListByProducerId(T::SessionId sessionId,uint producerId,std::set<std::string>& contentTimeList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getContentTimeListByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("contentTime",lines);
      for (uint t=0; t<len; t++)
      {
        contentTimeList.insert(lines[t]);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      count = response.getLineValueByKey("count");
    else
      count = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_setContentInfo(T::SessionId sessionId,T::ContentInfo& contentInfo)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","setContent");
    request.addLine("sessionId",sessionId);
    request.addLine("contentInfo",contentInfo.getCsv());

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getHashByProducerId(T::SessionId sessionId,uint producerId,ulonglong& hash)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getHashByProducerId");
    request.addLine("sessionId",sessionId);
    request.addLine("producerId",producerId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
      hash = response.getLineInt64ValueByKey("hash");
    else
      hash = 0;

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_getLevelInfoList(T::SessionId sessionId,T::LevelInfoList& levelInfoList)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","getLevelInfoList");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    if (result == Result::OK)
    {
      string_vec lines;
      uint len = response.getLinesByKey("levelInfo",lines);
      for (uint t=0; t<len; t++)
      {
        T::LevelInfo *info = new T::LevelInfo();
        info->setCsv(lines[t]);
        levelInfoList.addLevelInfo(info);
      }
    }

    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}



int ClientImplementation::_deleteVirtualContent(T::SessionId sessionId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","deleteVirtualContent");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





int ClientImplementation::_updateVirtualContent(T::SessionId sessionId)
{
  try
  {
    T::RequestMessage request;

    request.addLine("method","updateVirtualContent");
    request.addLine("sessionId",sessionId);

    T::ResponseMessage response;

    sendRequest(request,response);

    int result = response.getLineValueByKey("result");
    return result;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




size_t ClientImplementation_responseProcessing(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  try
  {
    T::ResponseMessage *response = (T::ResponseMessage*)userdata;

    for (size_t t=0; t<nmemb; t++)
    {
      char ch = ptr[t];
      response->addChar(ch);
    }
    return nmemb;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}






void ClientImplementation::sendRequest(T::RequestMessage& request,T::ResponseMessage& response)
{
  try
  {
    if (curl)
    {
      curl_easy_setopt(curl,CURLOPT_PROXY,"");
      curl_easy_setopt(curl, CURLOPT_URL,mUrl.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ClientImplementation_responseProcessing);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

      char *data = new char[request.getContentSize() + 100];
      char *p = data;

      std::unique_ptr<char[]> pData(data); // This should automatically release the allocated space

      uint lineCount = request.getLineCount();

      for (uint t=0; t<lineCount; t++)
      {
        std::string s = request.getLineByIndex(t);
        p += sprintf(p,"%s\n",s.c_str());
      }

      curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);

      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }

    response.addChar('\0');
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
}
