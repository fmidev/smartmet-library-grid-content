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

    virtual void addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);

    virtual void addVirtualContentFactory(VirtualContentFactory_ptr factory);

  protected:

    std::vector<VirtualContentFactory_ptr> mFactoryList;
};



}
}
