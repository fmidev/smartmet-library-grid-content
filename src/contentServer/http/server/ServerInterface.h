#pragma once

#include "contentServer/definition/ServiceInterface.h"
#include "contentServer/http/common/RequestMessage.h"
#include "contentServer/http/common/ResponseMessage.h"

namespace SmartMet
{
namespace ContentServer
{
namespace HTTP
{


class ServerInterface
{
  public:
                ServerInterface();
    virtual     ~ServerInterface();

    void        init(ContentServer::ServiceInterface *service);

    virtual void processRequest(T::RequestMessage& request,T::ResponseMessage& response);

  protected:

    virtual void clear(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void reload(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addDataServerInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteDataServerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getDataServerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getDataServerInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getDataServerInfoByIor(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getDataServerInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getDataServerInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addProducerInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addGenerationInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastGenerationInfoByProducerIdAndStatus(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastGenerationInfoByProducerNameAndStatus(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGenerationInfoStatusById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGenerationInfoStatusByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addFileInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addFileInfoWithContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addEventInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastEventInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getEventInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getEventInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void registerContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void registerContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void unregisterContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void unregisterContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByGroupFlags(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByServerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByGenerationIdAndTimeRange(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByGenerationNameAndTimeRange(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByParameter(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByParameterAndGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByParameterAndGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByParameterAndProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByParameterAndProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentCount(T::RequestMessage& request,T::ResponseMessage& response);

  protected:

    ContentServer::ServiceInterface *mService;
};


}
}
}
