#pragma once

#include "VirtualContentFactory.h"
#include "VirtualContentDefinitionFile.h"


namespace SmartMet
{
namespace DataServer
{


class VirtualContentFactory_type1 : public VirtualContentFactory
{
  public:
                VirtualContentFactory_type1();
    virtual     ~VirtualContentFactory_type1();

    void        init(std::string definitionFileName);
    void        addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList);

  protected:

    void        addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo);

  private:

    time_t      mLastCheck;

    VirtualContentDefinitionFile mContentDefinitionFile;
};



}
}
