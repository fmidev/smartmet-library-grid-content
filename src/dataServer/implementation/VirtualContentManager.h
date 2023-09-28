#pragma once

#include "VirtualContentFactory.h"
#include <vector>


namespace SmartMet
{
namespace DataServer
{


class VirtualContentManager
{
  public:
                 VirtualContentManager();
    virtual      ~VirtualContentManager();

    virtual void init();

    virtual void addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap);

    virtual void addVirtualContentFactory(VirtualContentFactory_ptr factory);

    virtual bool checkUpdates();

  protected:

    std::vector<VirtualContentFactory_ptr> mFactoryList;
};



}
}
