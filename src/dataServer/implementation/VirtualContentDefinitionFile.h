#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "VirtualContentDefinition.h"


namespace SmartMet
{
namespace DataServer
{


class VirtualContentDefinitionFile
{
  public:
                  VirtualContentDefinitionFile();
                  VirtualContentDefinitionFile(std::string filename);
                  VirtualContentDefinitionFile(const VirtualContentDefinitionFile& definitionList);
    virtual       ~VirtualContentDefinitionFile();

    virtual void  init();
    virtual void  init(std::string filename);
    virtual void  checkUpdates();
    virtual void  getContentDefinitions(std::string sourceParamName,std::string producerName,VirtualContentDefinition_vec& definitions);
    virtual void  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void  loadFile();

    std::string   mFilename;
    time_t        mLastModified;
    ThreadLock    mThreadLock;

    VirtualContentDefinition_vec mContentDefList;
};


typedef std::vector<VirtualContentDefinitionFile> ParamConversionFile_vec;


}  // namespace DataServer
}  // namespace SmartMet
