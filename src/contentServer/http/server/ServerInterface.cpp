#include "ServerInterface.h"
#include "grid-files/common/ShowFunction.h"


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace ContentServer
{
namespace HTTP
{


ServerInterface::ServerInterface()
{
  FUNCTION_TRACE
  try
  {
    mService = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




ServerInterface::~ServerInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::init(ContentServer::ServiceInterface *service)
{
  FUNCTION_TRACE
  try
  {
    if (service == NULL)
      throw SmartMet::Spine::Exception(BCP,"The 'service' parameter points to NULL!");

    mService = service;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::processRequest(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    if (mService == NULL)
      throw SmartMet::Spine::Exception(BCP,"The server not initialized!");

    std::string methodStr;

    if (!request.getLineByKey("method",methodStr))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: method");
      return;
    }


    const char *method = methodStr.c_str();

    // printf("[%s]\n",method);

    if (strcasecmp(method,"crash") == 0)
    {
      sprintf(NULL,"Crash");
      return;
    }

    if (strcasecmp(method,"clear") == 0)
    {
      clear(request,response);
      return;
    }

    if (strcasecmp(method,"reload") == 0)
    {
      reload(request,response);
      return;
    }

    if (strcasecmp(method,"addDataServerInfo") == 0)
    {
      addDataServerInfo(request,response);
      return;
    }

    if (strcasecmp(method,"deleteDataServerInfoById") == 0)
    {
      deleteDataServerInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"getDataServerInfoById") == 0)
    {
      getDataServerInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"getDataServerInfoByName") == 0)
    {
      getDataServerInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"getDataServerInfoByIor") == 0)
    {
      getDataServerInfoByIor(request,response);
      return;
    }

    if (strcasecmp(method,"getDataServerInfoList") == 0)
    {
      getDataServerInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"getDataServerInfoCount") == 0)
    {
      getDataServerInfoCount(request,response);
      return;
    }

    if (strcasecmp(method,"addProducerInfo") == 0)
    {
      addProducerInfo(request,response);
      return;
    }

    if (strcasecmp(method,"deleteProducerInfoById") == 0)
    {
      deleteProducerInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"deleteProducerInfoByName") == 0)
    {
      deleteProducerInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteProducerInfoListBySourceId") == 0)
    {
      deleteProducerInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerInfoById") == 0)
    {
      getProducerInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerInfoByName") == 0)
    {
      getProducerInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerInfoList") == 0)
    {
      getProducerInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerInfoListBySourceId") == 0)
    {
      getProducerInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerInfoCount") == 0)
    {
      getProducerInfoCount(request,response);
      return;
    }

    if (strcasecmp(method,"addGenerationInfo") == 0)
    {
      addGenerationInfo(request,response);
      return;
    }

    if (strcasecmp(method,"deleteGenerationInfoById") == 0)
    {
      deleteGenerationInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"deleteGenerationInfoByName") == 0)
    {
      deleteGenerationInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteGenerationInfoListByProducerId") == 0)
    {
      deleteGenerationInfoListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteGenerationInfoListByProducerName") == 0)
    {
      deleteGenerationInfoListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteGenerationInfoListBySourceId") == 0)
    {
      deleteGenerationInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoById") == 0)
    {
      getGenerationInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoByName") == 0)
    {
      getGenerationInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoList") == 0)
    {
      getGenerationInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoListByProducerId") == 0)
    {
      getGenerationInfoListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoListByProducerName") == 0)
    {
      getGenerationInfoListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoListBySourceId") == 0)
    {
      getGenerationInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getLastGenerationInfoByProducerIdAndStatus") == 0)
    {
      getLastGenerationInfoByProducerIdAndStatus(request,response);
      return;
    }

    if (strcasecmp(method,"getLastGenerationInfoByProducerNameAndStatus") == 0)
    {
      getLastGenerationInfoByProducerNameAndStatus(request,response);
      return;
    }

    if (strcasecmp(method,"getGenerationInfoCount") == 0)
    {
      getGenerationInfoCount(request,response);
      return;
    }

    if (strcasecmp(method,"setGenerationInfoStatusById") == 0)
    {
      setGenerationInfoStatusById(request,response);
      return;
    }

    if (strcasecmp(method,"setGenerationInfoStatusByName") == 0)
    {
      setGenerationInfoStatusByName(request,response);
      return;
    }

    if (strcasecmp(method,"addFileInfo") == 0)
    {
      addFileInfo(request,response);
      return;
    }

    if (strcasecmp(method,"addFileInfoWithContentList") == 0)
    {
      addFileInfoWithContentList(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoById") == 0)
    {
      deleteFileInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoByName") == 0)
    {
      deleteFileInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListByGroupFlags") == 0)
    {
      deleteFileInfoListByGroupFlags(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListByProducerId") == 0)
    {
      deleteFileInfoListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListByProducerName") == 0)
    {
      deleteFileInfoListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListByGenerationId") == 0)
    {
      deleteFileInfoListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListByGenerationName") == 0)
    {
      deleteFileInfoListByGenerationName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteFileInfoListBySourceId") == 0)
    {
      deleteFileInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoById") == 0)
    {
      getFileInfoById(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoByName") == 0)
    {
      getFileInfoByName(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoList") == 0)
    {
      getFileInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListByProducerId") == 0)
    {
      getFileInfoListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListByProducerName") == 0)
    {
      getFileInfoListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListByGenerationId") == 0)
    {
      getFileInfoListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListByGenerationName") == 0)
    {
      getFileInfoListByGenerationName(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListByGroupFlags") == 0)
    {
      getFileInfoListByGroupFlags(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoListBySourceId") == 0)
    {
      getFileInfoListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoCount") == 0)
    {
      getFileInfoCount(request,response);
      return;
    }

    if (strcasecmp(method,"addEventInfo") == 0)
    {
      addEventInfo(request,response);
      return;
    }

    if (strcasecmp(method,"getLastEventInfo") == 0)
    {
      getLastEventInfo(request,response);
      return;
    }

    if (strcasecmp(method,"getEventInfoList") == 0)
    {
      getEventInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"getEventInfoCount") == 0)
    {
      getEventInfoCount(request,response);
      return;
    }

    if (strcasecmp(method,"addContent") == 0)
    {
      addContentInfo(request,response);
      return;
    }

    if (strcasecmp(method,"addContentList") == 0)
    {
      addContentList(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentInfo") == 0)
    {
      deleteContentInfo(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByFileId") == 0)
    {
      deleteContentListByFileId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByFileName") == 0)
    {
      deleteContentListByFileName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByGroupFlags") == 0)
    {
      deleteContentListByGroupFlags(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByProducerId") == 0)
    {
      deleteContentListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByProducerName") == 0)
    {
      deleteContentListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByGenerationId") == 0)
    {
      deleteContentListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListByGenerationName") == 0)
    {
      deleteContentListByGenerationName(request,response);
      return;
    }

    if (strcasecmp(method,"deleteContentListBySourceId") == 0)
    {
      deleteContentListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"registerContentList") == 0)
    {
      registerContentList(request,response);
      return;
    }

    if (strcasecmp(method,"registerContentListByFileId") == 0)
    {
      registerContentListByFileId(request,response);
      return;
    }

    if (strcasecmp(method,"unregisterContentList") == 0)
    {
      unregisterContentList(request,response);
      return;
    }

    if (strcasecmp(method,"unregisterContentListByFileId") == 0)
    {
      unregisterContentListByFileId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentInfo") == 0)
    {
      getContentInfo(request,response);
      return;
    }

    if (strcasecmp(method,"getContentList") == 0)
    {
      getContentList(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByFileId") == 0)
    {
      getContentListByFileId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByFileName") == 0)
    {
      getContentListByFileName(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByGroupFlags") == 0)
    {
      getContentListByGroupFlags(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByProducerId") == 0)
    {
      getContentListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByProducerName") == 0)
    {
      getContentListByProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByServerId") == 0)
    {
      getContentListByServerId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByGenerationId") == 0)
    {
      getContentListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByGenerationName") == 0)
    {
      getContentListByGenerationName(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByGenerationIdAndTimeRange") == 0)
    {
      getContentListByGenerationIdAndTimeRange(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByGenerationNameAndTimeRange") == 0)
    {
      getContentListByGenerationNameAndTimeRange(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListBySourceId") == 0)
    {
      getContentListBySourceId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByParameter") == 0)
    {
      getContentListByParameter(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByParameterAndGenerationId") == 0)
    {
      getContentListByParameterAndGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByParameterAndGenerationName") == 0)
    {
      getContentListByParameterAndGenerationName(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByParameterAndProducerId") == 0)
    {
      getContentListByParameterAndProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentListByParameterAndProducerName") == 0)
    {
      getContentListByParameterAndProducerName(request,response);
      return;
    }

    if (strcasecmp(method,"getContentCount") == 0)
    {
      getContentCount(request,response);
      return;
    }

    response.addLine("result",(int)Result::UNKNOWN_METHOD);
    response.addLine("resultString","Unknown method: " + methodStr);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::clear(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    int result = mService->clear(sessionId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::reload(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    int result = mService->reload(sessionId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addDataServerInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("serverInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverInfo");
      return;
    }

    T::ServerInfo serverInfo(csv.c_str());

    int result = mService->addDataServerInfo(sessionId,serverInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("serverInfoHeader",serverInfo.getCsvHeader());
      response.addLine("serverInfo",serverInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteDataServerInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("severId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    int result = mService->deleteDataServerInfoById(sessionId,serverId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getDataServerInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    T::ServerInfo serverInfo;

    int result = mService->getDataServerInfoById(sessionId,serverId,serverInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("serverInfoHeader",serverInfo.getCsvHeader());
      response.addLine("serverInfo",serverInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getDataServerInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string serverName;
    if (!request.getLineByKey("serverName",serverName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverName");
      return;
    }

    T::ServerInfo serverInfo;

    int result = mService->getDataServerInfoByName(sessionId,serverName,serverInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("serverInfoHeader",serverInfo.getCsvHeader());
      response.addLine("serverInfo",serverInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getDataServerInfoByIor(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string serverIor;
    if (!request.getLineByKey("serverIor",serverIor))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverIor");
      return;
    }

    T::ServerInfo serverInfo;

    int result = mService->getDataServerInfoByIor(sessionId,serverIor,serverInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("serverInfoHeader",serverInfo.getCsvHeader());
      response.addLine("serverInfo",serverInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getDataServerInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    T::ServerInfoList serverInfoList;

    int result = mService->getDataServerInfoList(sessionId,serverInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = serverInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ServerInfo *info = serverInfoList.getServerInfoByIndex(t);
        if (t == 0)
          response.addLine("serverInfoHeader",info->getCsvHeader());
        response.addLine("serverInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getDataServerInfoCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getDataServerInfoCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addProducerInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("producerInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerInfo");
      return;
    }

    T::ProducerInfo producerInfo(csv.c_str());

    int result = mService->addProducerInfo(sessionId,producerInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("producerInfoHeader",producerInfo.getCsvHeader());
      response.addLine("producerInfo",producerInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    int result = mService->deleteProducerInfoById(sessionId,producerId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    int result = mService->deleteProducerInfoByName(sessionId,producerName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    int result = mService->deleteProducerInfoListBySourceId(sessionId,sourceId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    T::ProducerInfo producerInfo;

    int result = mService->getProducerInfoById(sessionId,producerId,producerInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("producerInfoHeader",producerInfo.getCsvHeader());
      response.addLine("producerInfo",producerInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    T::ProducerInfo producerInfo;

    int result = mService->getProducerInfoByName(sessionId,producerName,producerInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("producerInfoHeader",producerInfo.getCsvHeader());
      response.addLine("producerInfo",producerInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getProducerInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    T::ProducerInfoList producerInfoList;
    int result = mService->getProducerInfoList(sessionId,producerInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = producerInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ProducerInfo *info = producerInfoList.getProducerInfoByIndex(t);
        if (t == 0)
          response.addLine("producerInfoHeader",info->getCsvHeader());
        response.addLine("producerInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    T::ProducerInfoList producerInfoList;
    int result = mService->getProducerInfoListBySourceId(sessionId,sourceId,producerInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = producerInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ProducerInfo *info = producerInfoList.getProducerInfoByIndex(t);
        if (t == 0)
          response.addLine("producerInfoHeader",info->getCsvHeader());
        response.addLine("producerInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getProducerInfoCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getProducerInfoCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addGenerationInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("generationInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationInfo");
      return;
    }

    T::GenerationInfo generationInfo(csv.c_str());

    int result = mService->addGenerationInfo(sessionId,generationInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("generationInfoHeader",generationInfo.getCsvHeader());
      response.addLine("generationInfo",generationInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    int result = mService->deleteGenerationInfoById(sessionId,generationId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }


    int result = mService->deleteGenerationInfoByName(sessionId,generationName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    int result = mService->deleteGenerationInfoListByProducerId(sessionId,producerId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    int result = mService->deleteGenerationInfoListByProducerName(sessionId,producerName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    int result = mService->deleteGenerationInfoListBySourceId(sessionId,sourceId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getGenerationInfoById(sessionId,generationId,generationInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("generationInfoHeader",generationInfo.getCsvHeader());
      response.addLine("generationInfo",generationInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getGenerationInfoByName(sessionId,generationName,generationInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("generationInfoHeader",generationInfo.getCsvHeader());
      response.addLine("generationInfo",generationInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    T::GenerationInfoList generationInfoList;

    int result = mService->getGenerationInfoList(sessionId,generationInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = generationInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = generationInfoList.getGenerationInfoByIndex(t);
        if (t == 0)
          response.addLine("generationInfoHeader",info->getCsvHeader());
        response.addLine("generationInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    T::GenerationInfoList generationInfoList;

    int result = mService->getGenerationInfoListByProducerId(sessionId,producerId,generationInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = generationInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = generationInfoList.getGenerationInfoByIndex(t);
        if (t == 0)
          response.addLine("generationInfoHeader",info->getCsvHeader());
        response.addLine("generationInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    T::GenerationInfoList generationInfoList;

    int result = mService->getGenerationInfoListByProducerName(sessionId,producerName,generationInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = generationInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = generationInfoList.getGenerationInfoByIndex(t);
        if (t == 0)
          response.addLine("generationInfoHeader",info->getCsvHeader());
        response.addLine("generationInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    T::GenerationInfoList generationInfoList;

    int result = mService->getGenerationInfoListBySourceId(sessionId,sourceId,generationInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = generationInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::GenerationInfo *info = generationInfoList.getGenerationInfoByIndex(t);
        if (t == 0)
          response.addLine("generationInfoHeader",info->getCsvHeader());
        response.addLine("generationInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getLastGenerationInfoByProducerIdAndStatus(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint generationStatus = 0;
    if (!request.getLineByKey("generationStatus",generationStatus))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationStatus");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,(T::GenerationStatus)generationStatus,generationInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("generationInfoHeader",generationInfo.getCsvHeader());
      response.addLine("generationInfo",generationInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServerInterface::getLastGenerationInfoByProducerNameAndStatus(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint generationStatus = 0;
    if (!request.getLineByKey("generationStatus",generationStatus))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationStatus");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,(T::GenerationStatus)generationStatus,generationInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("generationInfoHeader",generationInfo.getCsvHeader());
      response.addLine("generationInfo",generationInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServerInterface::getGenerationInfoCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getGenerationInfoCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::setGenerationInfoStatusById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint status = 0;
    if (!request.getLineByKey("status",status))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: status");
      return;
    }

    int result = mService->setGenerationInfoStatusById(sessionId,generationId,(T::GenerationStatus)status);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::setGenerationInfoStatusByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint status = 0;
    if (!request.getLineByKey("status",status))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: status");
      return;
    }

    int result = mService->setGenerationInfoStatusByName(sessionId,generationName,(T::GenerationStatus)status);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServerInterface::addFileInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("fileInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileInfo");
      return;
    }

    T::FileInfo fileInfo(csv.c_str());

    int result = mService->addFileInfo(sessionId,fileInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("fileInfoHeader",fileInfo.getCsvHeader());
      response.addLine("fileInfo",fileInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addFileInfoWithContentList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("fileInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileInfo");
      return;
    }

    T::FileInfo fileInfo(csv.c_str());

    std::vector<std::string> csvLines;
    request.getLinesByKey("contentInfo",csvLines);
/*
    if (request.getLinesByKey("contentInfo",csvLines) == 0)
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: contentInfo");
      return;
    }
*/
    T::ContentInfoList contentInfoList;
    uint len = (uint)csvLines.size();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = new T::ContentInfo(csvLines[t].c_str());
      contentInfoList.addContentInfo(info);
    }

    int result = mService->addFileInfoWithContentList(sessionId,fileInfo,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("fileInfoHeader",fileInfo.getCsvHeader());
      response.addLine("fileInfo",fileInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    int result = mService->deleteFileInfoById(sessionId,fileId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    int result = mService->deleteFileInfoByName(sessionId,filename);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    int result = mService->deleteFileInfoListByGroupFlags(sessionId,groupFlags);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    int result = mService->deleteFileInfoListByProducerId(sessionId,producerId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    int result = mService->deleteFileInfoListByProducerName(sessionId,producerName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    int result = mService->deleteFileInfoListByGenerationId(sessionId,generationId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    int result = mService->deleteFileInfoListByGenerationName(sessionId,generationName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    int result = mService->deleteFileInfoListBySourceId(sessionId,sourceId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoById(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    T::FileInfo fileInfo;

    int result = mService->getFileInfoById(sessionId,fileId,fileInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("fileInfoHeader",fileInfo.getCsvHeader());
      response.addLine("fileInfo",fileInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: filename");
      return;
    }

    T::FileInfo fileInfo;

    int result = mService->getFileInfoByName(sessionId,filename,fileInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("fileInfoHeader",fileInfo.getCsvHeader());
      response.addLine("fileInfo",fileInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoList(sessionId,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,fileInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = fileInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *info = fileInfoList.getFileInfoByIndex(t);
        if (t == 0)
          response.addLine("fileInfoHeader",info->getCsvHeader());
        response.addLine("fileInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getFileInfoCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getFileInfoCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addEventInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("eventInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: eventInfo");
      return;
    }

    T::EventInfo eventInfo(csv.c_str());

    int result = mService->addEventInfo(sessionId,eventInfo);

    response.addLine("result",result);
    if (result == Result::OK)
      response.addLine("eventInfo",eventInfo.getCsv());
    else
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getLastEventInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint requestingServerId = 0;
    if (!request.getLineByKey("requestingServerId",requestingServerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestingServerId");
      return;
    }

    T::EventInfo eventInfo;

    int result = mService->getLastEventInfo(sessionId,requestingServerId,eventInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("eventInfoHeader",eventInfo.getCsvHeader());
      response.addLine("eventInfo",eventInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getEventInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint requestingServerId = 0;
    if (!request.getLineByKey("requestingServerId",requestingServerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestingServerId");
      return;
    }

    T::EventId startEventId = 0;
    if (!request.getLineByKey("startEventId",startEventId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startEventId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::EventInfoList eventInfoList;

    int result = mService->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      T::EventInfo *info = eventInfoList.getFirstEvent();
      if (info != NULL)
        response.addLine("eventInfoHeader",info->getCsvHeader());

      while (info != NULL)
      {
        response.addLine("eventInfo",info->getCsv());
        info = info->nextItem;
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getEventInfoCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getEventInfoCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addContentInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("contentInfo",csv))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: contentInfo");
      return;
    }

    T::ContentInfo contentInfo(csv.c_str());

    int result = mService->addContentInfo(sessionId,contentInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("contentInfoHeader",contentInfo.getCsvHeader());
      response.addLine("contentInfo",contentInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::addContentList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::vector<std::string> csvLines;
    if (request.getLinesByKey("contentInfo",csvLines) == 0)
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: contentInfo");
      return;
    }

    T::ContentInfoList contentInfoList;
    uint len = (uint)csvLines.size();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *info = new T::ContentInfo(csvLines[t].c_str());
      contentInfoList.addContentInfo(info);
    }

    int result = mService->addContentList(sessionId,contentInfoList);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint messageIndex = 0;
    if (!request.getLineByKey("messageIndex",messageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: messageIndex");
      return;
    }

    int result = mService->deleteContentInfo(sessionId,fileId,messageIndex);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    int result = mService->deleteContentListByFileId(sessionId,fileId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: filename");
      return;
    }

    int result = mService->deleteContentListByFileName(sessionId,filename);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    int result = mService->deleteContentListByGroupFlags(sessionId,groupFlags);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    int result = mService->deleteContentListByProducerId(sessionId,producerId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    int result = mService->deleteContentListByProducerName(sessionId,producerName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    int result = mService->deleteContentListByGenerationId(sessionId,generationId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    int result = mService->deleteContentListByGenerationName(sessionId,generationName);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::deleteContentListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    int result = mService->deleteContentListBySourceId(sessionId,sourceId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::registerContentList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->registerContentList(sessionId,serverId,contentInfoList);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::registerContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    int result = mService->registerContentListByFileId(sessionId,serverId,fileId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::unregisterContentList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    int result = mService->unregisterContentList(sessionId,serverId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::unregisterContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    int result = mService->unregisterContentListByFileId(sessionId,serverId,fileId);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentInfo(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint messageIndex = 0;
    if (!request.getLineByKey("messageIndex",messageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: messageIndex");
      return;
    }

    T::ContentInfo contentInfo;

    int result = mService->getContentInfo(sessionId,fileId,messageIndex,contentInfo);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("contentInfoHeader",contentInfo.getCsvHeader());
      response.addLine("contentInfo",contentInfo.getCsv());
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentList(sessionId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByFileId(sessionId,fileId,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: filename");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByFileName(sessionId,filename,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByServerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByServerId(sessionId,serverId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByGenerationIdAndTimeRange(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByGenerationNameAndTimeRange(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByParameter(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameter(sessionId,(T::ParamKeyType)parameterKeyType,
        parameterKey,(T::ParamLevelIdType)parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        startTime,endTime,requestFlags,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByParameterAndGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndGenerationId(sessionId,generationId,(T::ParamKeyType)parameterKeyType,
        parameterKey,(T::ParamLevelIdType)parameterLevelIdType,(T::ParamLevelId)parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        startTime,endTime,requestFlags,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByParameterAndGenerationName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndGenerationName(sessionId,generationName,(T::ParamKeyType)parameterKeyType,
        parameterKey,(T::ParamLevelIdType)parameterLevelIdType,(T::ParamLevelId)parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        startTime,endTime,requestFlags,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentListByParameterAndProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndProducerId(sessionId,producerId,(T::ParamKeyType)parameterKeyType,
        parameterKey,(T::ParamLevelIdType)parameterLevelIdType,(T::ParamLevelId)parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        startTime,endTime,requestFlags,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ServerInterface::getContentListByParameterAndProducerName(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndProducerName(sessionId,producerName,(T::ParamKeyType)parameterKeyType,
        parameterKey,(T::ParamLevelIdType)parameterLevelIdType,(T::ParamLevelId)parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        startTime,endTime,requestFlags,contentInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentInfoList.getContentInfoByIndex(t);
        if (t == 0)
          response.addLine("contentInfoHeader",info->getCsvHeader());
        response.addLine("contentInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServerInterface::getContentCount(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",(int)Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint count = 0;

    int result = mService->getContentCount(sessionId,count);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("count",count);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



}
}
}
