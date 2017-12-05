#pragma once

#include "VirtualContentFactory.h"
#include "VirtualContentDefinitionFile.h"


namespace SmartMet
{
namespace DataServer
{

#define CVF_NAME_SIZE 200


class VirtualContentFactory_type1 : public VirtualContentFactory
{
  public:
                VirtualContentFactory_type1();
    virtual     ~VirtualContentFactory_type1();

    void        init(std::string definitionFileName);
    void        addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap);

  protected:

    void        addContent(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap);

  private:

    time_t      mLastCheck;
    std::string mFileNames[CVF_NAME_SIZE];
    uint        mFileCounter;

    VirtualContentDefinitionFile mContentDefinitionFile;
};



}
}
