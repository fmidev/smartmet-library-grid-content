#pragma once

#include "../definition/ServiceInterface.h"


namespace SmartMet
{

namespace ContentServer
{


class ContentSync
{
  public:
                            ContentSync();
     virtual                ~ContentSync();

     virtual void           synchronize(T::SessionId sessionId,ServiceInterface *sourceInterface,ServiceInterface *targetInterface,uint sourceId,uint targetId);
     virtual void           synchronize(T::SessionId sessionId,const char *sourceDir,ServiceInterface *targetInterface,uint sourceId,uint targetId);

  protected:

     virtual void           readSourceProducers(ServiceInterface *sourceInterface);
     virtual void           readSourceGenerations(ServiceInterface *sourceInterface);
     virtual void           readSourceFiles(ServiceInterface *sourceInterface);
     virtual void           readSourceContent(ServiceInterface *sourceInterface);

     virtual void           readSourceProducers(const char *sourceDir);
     virtual void           readSourceGenerations(const char *sourceDir);
     virtual void           readSourceFiles(const char *sourceDir);
     virtual void           readSourceContent(const char *sourceDir);

     virtual void           readTargetProducers(ServiceInterface *targetInterface);
     virtual void           readTargetGenerations(ServiceInterface *targetInterface);
     virtual void           readTargetFiles(ServiceInterface *targetInterface);
     virtual void           readTargetContent(ServiceInterface *targetInterface);

     virtual void           updateProducers(ServiceInterface *targetInterface);
     virtual void           updateGenerations(ServiceInterface *targetInterface);
     virtual void           updateFiles(ServiceInterface *targetInterface);
     virtual void           updateContent(ServiceInterface *targetInterface);

     T::SessionId           mSessionId;
     //ServiceInterface*      mSourceInterface;
     //ServiceInterface*      mTargetInterface;
     uint                   mSourceId;
     uint                   mTargetId;

     T::ProducerInfoList    mSourceProducerList;
     T::GenerationInfoList  mSourceGenerationList;
     T::FileInfoList        mSourceFileList;
     T::ContentInfoList     mSourceContentList;

     T::ProducerInfoList    mTargetProducerList;
     T::GenerationInfoList  mTargetGenerationList;
     T::FileInfoList        mTargetFileList;
     T::ContentInfoList     mTargetContentList;

};



}
}
