#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoThreadLock.h>
#include "ParameterMapping.h"


namespace SmartMet
{
namespace QueryServer
{

typedef std::map<std::string,ParameterMapping_vec> MappingSearch;

class ParameterMappingFile
{
  public:
                          ParameterMappingFile(std::string filename);
                          ParameterMappingFile(const ParameterMappingFile& mappingFile);
    virtual               ~ParameterMappingFile();

    virtual void          init();
    virtual bool          checkUpdates();
    std::string           getFilename();
    ParameterMapping*     getMapping(ParameterMapping& mapping);
    virtual void          getMappings(std::string producerName,std::string parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(std::string producerName,std::string parameterName,T::ParamLevelIdType levelIdType,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    uint                  getNumberOfMappings();
    //ParameterMapping*     getParameterMappingByIndex(uint index);
    void                  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void          loadFile();

    std::string           mFilename;
    time_t                mLastModified;
    MappingSearch         mMappingSearch;
    //ParameterMapping_vec  mMappingVector;
    ThreadLock            mThreadLock;
};


typedef std::vector<ParameterMappingFile> ParamMappingFile_vec;


}  // namespace QueryServer
}  // namespace SmartMet
