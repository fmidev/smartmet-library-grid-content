#pragma once

#include "../../definition/ServiceInterface.h"
#include "../common/RequestMessage.h"
#include "../common/ResponseMessage.h"

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
    virtual void getContentChangeTime(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addProducerInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoListByParameter(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerNameAndGeometryList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerParameterList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getProducerParameterListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setProducerInfo(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addGenerationInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListByIdList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationIdGeometryIdAndForecastTimeList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListByGeometryId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastGenerationInfoByProducerIdAndStatus(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastGenerationInfoByProducerNameAndStatus(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGenerationInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGenerationInfoStatusById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGenerationInfoStatusByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGenerationInfo(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addGeometryInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGeometryInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGeometryInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGeometryInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteGeometryInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getGeometryInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGeometryInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setGeometryInfoStatusById(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addFileInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addFileInfoWithContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addFileInfoListWithContent(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByForecastTimeList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGenerationIdAndForecastTime(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteFileInfoListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoById(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoByName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoCountByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoCountByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getFileInfoCountBySourceId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setFileInfo(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addEventInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getLastEventInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getEventInfoList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getEventInfoCount(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void addContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void addContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListByGenerationName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void deleteContentListBySourceId(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void getContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByFileId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByFileIdList(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByFileName(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentListByProducerName(T::RequestMessage& request,T::ResponseMessage& response);
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
    virtual void getContentListByParameterGenerationIdAndForecastTime(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void getContentGeometryIdListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentLevelListByGenerationGeometryAndLevelId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentParamListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentParamKeyListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentParamKeyListByGenerationAndGeometryId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentParamKeyListByGenerationGeometryAndLevelId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeListByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeListByGenerationAndGeometryId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeListByGenerationGeometryAndLevelId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeListByProducerId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeRangeByProducerAndGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentTimeRangeByGenerationId(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getContentCount(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void setContentInfo(T::RequestMessage& request,T::ResponseMessage& response);
    virtual void getHashByProducerId(T::RequestMessage& request,T::ResponseMessage& response);

    virtual void getLevelInfoList(T::RequestMessage& request,T::ResponseMessage& response);

  protected:

    ContentServer::ServiceInterface *mService;
};


}
}
}
