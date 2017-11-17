#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "ParameterMapping.h"


namespace SmartMet
{
namespace QueryServer
{


class ParameterMappingFile
{
  public:
                          ParameterMappingFile(std::string filename);
                          ParameterMappingFile(const ParameterMappingFile& mappingList);
    virtual               ~ParameterMappingFile();

    virtual void          init();
    virtual void          checkUpdates();
    virtual void          getMappings(std::string producerName,std::string parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    void                  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void          loadFile();

    std::string           mFilename;
    time_t                mLastModified;
    ParameterMapping_vec  mMappingVector;
    ThreadLock            mThreadLock;
};


typedef std::vector<ParameterMappingFile> ParamMappingFile_vec;


}  // namespace QueryServer
}  // namespace SmartMet
