#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "ParameterMapping.h"


namespace SmartMet
{
namespace QueryServer
{


class ParameterMappingList
{
  public:
                          ParameterMappingList(std::string filename);
                          ParameterMappingList(const ParameterMappingList& mappingList);
    virtual               ~ParameterMappingList();

    virtual void          init();
    virtual void          checkUpdates();
    virtual void          getMappings(std::string producerName,std::string parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    void                  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void          load();

    std::string           mFilename;
    time_t                mLastModified;
    ParameterMapping_vec  mMappingVector;
    ThreadLock            mThreadLock;
};



}  // namespace QueryServer
}  // namespace SmartMet
