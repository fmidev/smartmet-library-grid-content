#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include "ParameterMapping.h"
#include <map>


namespace SmartMet
{
namespace QueryServer
{

typedef std::map<std::string,ParameterMapping_vec> MappingSearch;
typedef std::shared_ptr<MappingSearch> MappingSearch_sptr;

class ParameterMappingFile
{
  public:
                          ParameterMappingFile(const std::string& filename);
                          //ParameterMappingFile(const ParameterMappingFile& mappingFile);
    virtual               ~ParameterMappingFile();

    virtual void          init();
    virtual bool          checkUpdates();
    std::string           getFilename();
    ParameterMapping*     getMapping(ParameterMapping& mapping);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    uint                  getNumberOfMappings();
    void                  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void          loadFile();

    std::string           mFilename;
    time_t                mLastModified;
    MappingSearch_sptr    mMappingSearch;
    ModificationLock      mModificationLock;
    ThreadLock            mFileReadLock;
};


typedef std::vector<ParameterMappingFile> ParamMappingFile_vec;


}  // namespace QueryServer
}  // namespace SmartMet
