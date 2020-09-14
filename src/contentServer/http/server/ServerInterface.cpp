#include "ServerInterface.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>


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
    mService = nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServerInterface::init(ContentServer::ServiceInterface *service)
{
  FUNCTION_TRACE
  try
  {
    if (service == nullptr)
      throw SmartMet::Spine::Exception(BCP,"The 'service' parameter points to nullptr!");

    mService = service;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::processRequest(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw SmartMet::Spine::Exception(BCP,"The server not initialized!");

    std::string methodStr;

    if (!request.getLineByKey("method",methodStr))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: method");
      return;
    }


    const char *method = methodStr.c_str();

    // printf("[%s]\n",method);
    //request.print(std::cout,0,0);

    /* This is for debugging purposes. I.e. you can crash the system and get a core dump for
     * example when there is a deadlock or a busy loop that cannot be otherways found.

    if (strcasecmp(method,"crash") == 0)
    {
      sprintf(nullptr,"Crash");
      return;
    }
    */

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

    if (strcasecmp(method,"getProducerNameAndGeometryList") == 0)
    {
      getProducerNameAndGeometryList(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerParameterList") == 0)
    {
      getProducerParameterList(request,response);
      return;
    }

    if (strcasecmp(method,"getProducerParameterListByProducerId") == 0)
    {
      getProducerParameterListByProducerId(request,response);
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

    if (strcasecmp(method,"deleteGenerationInfoListByIdList") == 0)
    {
      deleteGenerationInfoListByIdList(request,response);
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

    if (strcasecmp(method,"getGenerationIdGeometryIdAndForecastTimeList") == 0)
    {
      getGenerationIdGeometryIdAndForecastTimeList(request,response);
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

    if (strcasecmp(method,"getGenerationInfoListByGeometryId") == 0)
    {
      getGenerationInfoListByGeometryId(request,response);
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

    if (strcasecmp(method,"addFileInfoListWithContent") == 0)
    {
      //request.print(std::cout,0,0);
      addFileInfoListWithContent(request,response);
      //response.print(std::cout,0,0);
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

    if (strcasecmp(method,"deleteFileInfoListByForecastTimeList") == 0)
    {
      deleteFileInfoListByForecastTimeList(request,response);
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

    if (strcasecmp(method,"deleteFileInfoListByFileIdList") == 0)
    {
      deleteFileInfoListByFileIdList(request,response);
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

    if (strcasecmp(method,"getFileInfoListByFileIdList") == 0)
    {
      getFileInfoListByFileIdList(request,response);
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

    if (strcasecmp(method,"getFileInfoCountByProducerId") == 0)
    {
      getFileInfoCountByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoCountByGenerationId") == 0)
    {
      getFileInfoCountByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getFileInfoCountBySourceId") == 0)
    {
      getFileInfoCountBySourceId(request,response);
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

    if (strcasecmp(method,"getContentListByFileIdList") == 0)
    {
      getContentListByFileIdList(request,response);
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

    if (strcasecmp(method,"getContentListByParameterGenerationIdAndForecastTime") == 0)
    {
      getContentListByParameterGenerationIdAndForecastTime(request,response);
      return;
    }

    if (strcasecmp(method,"getContentGeometryIdListByGenerationId") == 0)
    {
      getContentGeometryIdListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentParamListByGenerationId") == 0)
    {
      getContentParamListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentParamKeyListByGenerationId") == 0)
    {
      getContentParamKeyListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentTimeListByGenerationId") == 0)
    {
      getContentTimeListByGenerationId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentTimeListByGenerationAndGeometryId") == 0)
    {
      getContentTimeListByGenerationAndGeometryId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentTimeListByProducerId") == 0)
    {
      getContentTimeListByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getContentCount") == 0)
    {
      getContentCount(request,response);
      return;
    }

    if (strcasecmp(method,"getHashByProducerId") == 0)
    {
      getHashByProducerId(request,response);
      return;
    }

    if (strcasecmp(method,"getLevelInfoList") == 0)
    {
      getLevelInfoList(request,response);
      return;
    }

    if (strcasecmp(method,"deleteVirtualContent") == 0)
    {
      deleteVirtualContent(request,response);
      return;
    }

    if (strcasecmp(method,"updateVirtualContent") == 0)
    {
      updateVirtualContent(request,response);
      return;
    }

    response.addLine("result",Result::UNKNOWN_METHOD);
    response.addLine("resultString","Unknown method: " + methodStr);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("serverInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("severId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string serverName;
    if (!request.getLineByKey("serverName",serverName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string serverIor;
    if (!request.getLineByKey("serverIor",serverIor))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("producerInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getProducerInfoListByParameter(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    unsigned char parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    T::ProducerInfoList producerInfoList;
    int result = mService->getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,producerInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getProducerNameAndGeometryList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::set<std::string> list;

    int result = mService->getProducerNameAndGeometryList(sessionId,list);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=list.begin(); it!=list.end(); ++it)
      {
        response.addLine("line",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getProducerParameterList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    int sourceParameterKeyType = 0;
    if (!request.getLineByKey("sourceParameterKeyType",sourceParameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceParameterKeyType");
      return;
    }

    int targetParameterKeyType = 0;
    if (!request.getLineByKey("targetParameterKeyType",targetParameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: targetParameterKeyType");
      return;
    }

    std::set<std::string> list;

    int result = mService->getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,list);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=list.begin(); it!=list.end(); ++it)
      {
        response.addLine("line",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getProducerParameterListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    int sourceParameterKeyType = 0;
    if (!request.getLineByKey("sourceParameterKeyType",sourceParameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceParameterKeyType");
      return;
    }

    int targetParameterKeyType = 0;
    if (!request.getLineByKey("targetParameterKeyType",targetParameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: targetParameterKeyType");
      return;
    }

    std::set<std::string> list;

    int result = mService->getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,list);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=list.begin(); it!=list.end(); ++it)
      {
        response.addLine("line",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("generationInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::deleteGenerationInfoListByIdList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    string_vec csvLines;
    if (request.getLinesByKey("generationId",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    std::set<uint> generationIdList;

    for (auto it=csvLines.begin(); it!=csvLines.end(); ++it)
    {
      generationIdList.insert(toInt64(it->c_str()));
    }

    int result = mService->deleteGenerationInfoListByIdList(sessionId,generationIdList);
    response.addLine("result",result);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getGenerationIdGeometryIdAndForecastTimeList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::set<std::string> list;

    int result = mService->getGenerationIdGeometryIdAndForecastTimeList(sessionId,list);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=list.begin(); it!=list.end(); ++it)
      {
        response.addLine("line",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getGenerationInfoListByGeometryId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    T::GenerationInfoList generationInfoList;

    int result = mService->getGenerationInfoListByGeometryId(sessionId,geometryId,generationInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint generationStatus = 0;
    if (!request.getLineByKey("generationStatus",generationStatus))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationStatus");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,generationInfo);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint generationStatus = 0;
    if (!request.getLineByKey("generationStatus",generationStatus))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationStatus");
      return;
    }

    T::GenerationInfo generationInfo;

    int result = mService->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,generationInfo);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint status = 0;
    if (!request.getLineByKey("status",status))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: status");
      return;
    }

    int result = mService->setGenerationInfoStatusById(sessionId,generationId,status);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint status = 0;
    if (!request.getLineByKey("status",status))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: status");
      return;
    }

    int result = mService->setGenerationInfoStatusByName(sessionId,generationName,status);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("fileInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("fileInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileInfo");
      return;
    }

    T::FileInfo fileInfo(csv.c_str());

    string_vec csvLines;
    request.getLinesByKey("contentInfo",csvLines);
/*
    if (request.getLinesByKey("contentInfo",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: contentInfo");
      return;
    }
*/
    T::ContentInfoList contentInfoList;
    uint len = csvLines.size();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::addFileInfoListWithContent(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    uint length = 0;
    if (!request.getLineByKey("length",length))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: length");
      return;
    }

    char msg[200];
    char tmp[200];

    std::vector<T::FileAndContent> fileAndContentList;

    for (uint f=0; f<length; f++)
    {
      T::FileAndContent fc;

      sprintf(tmp,"fileInfo.%u",f);
      std::string csv;
      if (!request.getLineByKey(tmp,csv))
      {
        response.addLine("result",Result::MISSING_PARAMETER);
        sprintf(msg,"Missing parameter: %s",tmp);
        response.addLine("resultString",msg);
        return;
      }

      fc.mFileInfo.setCsv(csv.c_str());

      sprintf(tmp,"contentInfo.%u",f);

      string_vec csvLines;
      request.getLinesByKey(tmp,csvLines);

      uint len = csvLines.size();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = new T::ContentInfo(csvLines[t].c_str());
        fc.mContentInfoList.addContentInfo(info);
      }

      fileAndContentList.push_back(fc);
    }

    int result = mService->addFileInfoListWithContent(sessionId,requestFlags,fileAndContentList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("length",length);
      for (uint f=0; f<length; f++)
      {
        sprintf(tmp,"fileInfo.%u",f);
        response.addLine(tmp,fileAndContentList[f].mFileInfo.getCsv());

        sprintf(tmp,"contentInfo.%u",f);

        uint len = fileAndContentList[f].mContentInfoList.getLength();
        for (uint t=0; t<len; t++)
        {
          T::ContentInfo *info = fileAndContentList[f].mContentInfoList.getContentInfoByIndex(t);
          response.addLine(tmp,info->getCsv());
        }
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::deleteFileInfoListByForecastTimeList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::vector<std::string> csvLines;
    request.getLinesByKey("forecastTime",csvLines);


    std::vector<T::ForecastTime> forecastTimeList;
    uint len = csvLines.size();
    for (uint t=0; t<len; t++)
    {
      T::ForecastTime ft(csvLines[t].c_str());
      forecastTimeList.push_back(ft);
    }

    int result = mService->deleteFileInfoListByForecastTimeList(sessionId,forecastTimeList);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::deleteFileInfoListByGenerationIdAndForecastTime(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    short forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    short forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    std::string forecastTime;
    if (!request.getLineByKey("forecastTime",forecastTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastTime");
      return;
    }


    int result = mService->deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);

    response.addLine("result",result);

    if (result != Result::OK)
      response.addLine("resultString",getResultString(result));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getFileInfoListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    string_vec csvLines;
    if (request.getLinesByKey("fileId",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    std::vector<uint> fileIdList;
    for (auto it=csvLines.begin(); it!=csvLines.end(); ++it)
    {
      fileIdList.push_back(toInt64(it->c_str()));
    }

    T::FileInfoList fileInfoList;

    int result = mService->getFileInfoListByFileIdList(sessionId,fileIdList,fileInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::deleteFileInfoListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    string_vec csvLines;
    if (request.getLinesByKey("fileId",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    std::set<uint> fileIdList;

    for (auto it=csvLines.begin(); it!=csvLines.end(); ++it)
    {
      fileIdList.insert(toInt64(it->c_str()));
    }

    int result = mService->deleteFileInfoListByFileIdList(sessionId,fileIdList);
    response.addLine("result",result);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getFileInfoCountByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint count = 0;

    int result = mService->getFileInfoCountByProducerId(sessionId,producerId,count);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getFileInfoCountByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint count = 0;

    int result = mService->getFileInfoCountByGenerationId(sessionId,generationId,count);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getFileInfoCountBySourceId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    uint count = 0;

    int result = mService->getFileInfoCountBySourceId(sessionId,sourceId,count);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("eventInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint requestingServerId = 0;
    if (!request.getLineByKey("requestingServerId",requestingServerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint requestingServerId = 0;
    if (!request.getLineByKey("requestingServerId",requestingServerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestingServerId");
      return;
    }

    T::EventId startEventId = 0;
    if (!request.getLineByKey("startEventId",startEventId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startEventId");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    T::EventInfoList eventInfoList;

    int result = mService->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,eventInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      T::EventInfo *info = eventInfoList.getFirstEvent();
      if (info != nullptr)
        response.addLine("eventInfoHeader",info->getCsvHeader());

      while (info != nullptr)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string csv;
    if (!request.getLineByKey("contentInfo",csv))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    string_vec csvLines;
    if (request.getLinesByKey("contentInfo",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: contentInfo");
      return;
    }

    T::ContentInfoList contentInfoList;
    uint len = csvLines.size();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint messageIndex = 0;
    if (!request.getLineByKey("messageIndex",messageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    uint messageIndex = 0;
    if (!request.getLineByKey("messageIndex",messageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    string_vec csvLines;
    if (request.getLinesByKey("fileId",csvLines) == 0)
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: fileId");
      return;
    }

    std::vector<uint> fileIdList;
    for (auto it=csvLines.begin(); it!=csvLines.end(); ++it)
    {
      fileIdList.push_back(toInt64(it->c_str()));
    }


    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByFileIdList(sessionId,fileIdList,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint fileId = 0;
    if (!request.getLineByKey("fileId",fileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string filename;
    if (!request.getLineByKey("filename",filename))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint groupFlags = 0;
    if (!request.getLineByKey("groupFlags",groupFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: groupFlags");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint serverId = 0;
    if (!request.getLineByKey("serverId",serverId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: serverId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentListByRequestCounterKey(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    ulonglong key = 0;
    if (!request.getLineByKey("key",key))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: key");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByRequestCounterKey(sessionId,key,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxRecords");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint sourceId = 0;
    if (!request.getLineByKey("sourceId",sourceId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sourceId");
      return;
    }

    uint startFileId = 0;
    if (!request.getLineByKey("startFileId",startFileId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startFileId");
      return;
    }

    uint startMessageIndex = 0;
    if (!request.getLineByKey("startMessageIndex",startMessageIndex))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startMessageIndex");
      return;
    }

    uint maxRecords = 0;
    if (!request.getLineByKey("maxRecords",maxRecords))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameter(sessionId,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string generationName;
    if (!request.getLineByKey("generationName",generationName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    std::string producerName;
    if (!request.getLineByKey("producerName",producerName))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationName");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int minLevel = 0;
    if (!request.getLineByKey("minLevel",minLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: minLevel");
      return;
    }

    int maxLevel = 0;
    if (!request.getLineByKey("maxLevel",maxLevel))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: maxLevel");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string startTime;
    if (!request.getLineByKey("startTime",startTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: startTime");
      return;
    }

    std::string endTime;
    if (!request.getLineByKey("endTime",endTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: endTime");
      return;
    }

    uint requestFlags = 0;
    if (!request.getLineByKey("requestFlags",requestFlags))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: requestFlags");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)minLevel,(T::ParamLevel)maxLevel,
        forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentListByParameterGenerationIdAndForecastTime(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    uint parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::string parameterKey;
    if (!request.getLineByKey("parameterKey",parameterKey))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKey");
      return;
    }

    uint parameterLevelIdType = 0;
    if (!request.getLineByKey("parameterLevelIdType",parameterLevelIdType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelIdType");
      return;
    }

    unsigned char parameterLevelId = 0;
    if (!request.getLineByKey("parameterLevelId",parameterLevelId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterLevelId");
      return;
    }

    int level = 0;
    if (!request.getLineByKey("level",level))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: level");
      return;
    }

    T::ForecastType forecastType = 0;
    if (!request.getLineByKey("forecastType",forecastType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastType");
      return;
    }

    T::ForecastNumber forecastNumber = 0;
    if (!request.getLineByKey("forecastNumber",forecastNumber))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastNumber");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::string forecastTime;
    if (!request.getLineByKey("forecastTime",forecastTime))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: forecastTime");
      return;
    }

    T::ContentInfoList contentInfoList;

    int result = mService->getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,
        parameterKey,parameterLevelIdType,parameterLevelId,(T::ParamLevel)level,
        forecastType,forecastNumber,geometryId,forecastTime,contentInfoList);

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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentGeometryIdListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    std::set<T::GeometryId> geometryIdList;

    int result = mService->getContentGeometryIdListByGenerationId(sessionId,generationId,geometryIdList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=geometryIdList.begin(); it!=geometryIdList.end(); ++it)
      {
        response.addLine("geometryId",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentParamListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    T::ContentInfoList contentParamList;

    int result = mService->getContentParamListByGenerationId(sessionId,generationId,contentParamList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = contentParamList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *info = contentParamList.getContentInfoByIndex(t);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentParamKeyListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    unsigned char parameterKeyType = 0;
    if (!request.getLineByKey("parameterKeyType",parameterKeyType))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: parameterKeyType");
      return;
    }

    std::set<std::string> paramKeyList;

    int result = mService->getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,paramKeyList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=paramKeyList.begin(); it!=paramKeyList.end(); ++it)
      {
        response.addLine("paramKey",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentTimeListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }


    std::set<std::string> contentTimeList;

    int result = mService->getContentTimeListByGenerationId(sessionId,generationId,contentTimeList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=contentTimeList.begin(); it!=contentTimeList.end(); ++it)
      {
        response.addLine("contentTime",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentTimeListByGenerationAndGeometryId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint generationId = 0;
    if (!request.getLineByKey("generationId",generationId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: generationId");
      return;
    }

    T::GeometryId geometryId = 0;
    if (!request.getLineByKey("geometryId",geometryId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: geometryId");
      return;
    }

    std::set<std::string> contentTimeList;

    int result = mService->getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,contentTimeList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=contentTimeList.begin(); it!=contentTimeList.end(); ++it)
      {
        response.addLine("contentTime",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getContentTimeListByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }


    std::set<std::string> contentTimeList;

    int result = mService->getContentTimeListByProducerId(sessionId,producerId,contentTimeList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      for (auto it=contentTimeList.begin(); it!=contentTimeList.end(); ++it)
      {
        response.addLine("contentTime",*it);
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
      response.addLine("result",Result::MISSING_PARAMETER);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getHashByProducerId(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    uint producerId = 0;
    if (!request.getLineByKey("producerId",producerId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: producerId");
      return;
    }

    ulonglong hash = 0;

    int result = mService->getHashByProducerId(sessionId,producerId,hash);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      response.addLine("hash",hash);
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::getLevelInfoList(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    T::LevelInfoList levelInfoList;

    int result = mService->getLevelInfoList(sessionId,levelInfoList);

    response.addLine("result",result);
    if (result == Result::OK)
    {
      uint len = levelInfoList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::LevelInfo *info = levelInfoList.getLevelInfoByIndex(t);
        if (t == 0)
          response.addLine("levelInfoHeader",info->getCsvHeader());
        response.addLine("levelInfo",info->getCsv());
      }
    }
    else
    {
      response.addLine("resultString",getResultString(result));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::deleteVirtualContent(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    int result = mService->deleteVirtualContent(sessionId);

    response.addLine("result",result);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ServerInterface::updateVirtualContent(T::RequestMessage& request,T::ResponseMessage& response)
{
  FUNCTION_TRACE
  try
  {
    T::SessionId sessionId = 0;
    if (!request.getLineByKey("sessionId",sessionId))
    {
      response.addLine("result",Result::MISSING_PARAMETER);
      response.addLine("resultString","Missing parameter: sessionId");
      return;
    }

    int result = mService->updateVirtualContent(sessionId);

    response.addLine("result",result);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
}
