#pragma once

#include <grid-files/grid/Typedefs.h>
#include <grid-files/common/AutoWriteLock.h>
#include <grid-files/common/AutoReadLock.h>
#include "ParameterMapping.h"
#include "UnitConversion.h"
#include <map>


namespace SmartMet
{
namespace QueryServer
{

using MappingSearch = std::map<std::string,ParameterMapping_vec>;        //!< Map from parameter name to matching ParameterMapping records.
using MappingSearch_sptr = std::shared_ptr<MappingSearch>;               //!< Shared pointer to a MappingSearch map.

// ====================================================================================
/*! \brief Loads and indexes a file of ParameterMapping definitions.
 *
 *  Reads mapping records from a text file and builds forward and reverse search
 *  indexes.  checkUpdates() hot-reloads when the file changes on disk.  Multiple
 *  getMappings() overloads allow filtering by producer, geometry, and level. */
// ====================================================================================

class ParameterMappingFile
{
  public:
                          ParameterMappingFile();
                          ParameterMappingFile(const std::string& filename);
                          //ParameterMappingFile(const ParameterMappingFile& mappingFile);
    virtual               ~ParameterMappingFile();

    virtual void          init();
    virtual bool          checkUpdates();
    virtual void          getAliasMappings(ParameterMapping_vec& mappings,UnitConversion_vec& unitConversions);
    std::string           getFilename();
    ParameterMapping*     getMapping(const ParameterMapping& mapping);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappings(const std::string& producerName,const std::string& parameterName,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamLevel level,bool onlySearchEnabled,ParameterMapping_vec& mappings);
    virtual void          getMappingsByParamKey(const std::string& producerName,T::ParamKeyType parameterKeyType,const std::string& parameterKey,T::GeometryId geometryId,T::ParamLevelId levelId,T::ParamLevel level,ParameterMapping_vec& mappings);
    uint                  getNumberOfMappings();
    void                  setParameterMappings(const ParameterMapping_vec& mappings);
    void                  print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    virtual void          loadFile();

    std::string           mFilename;              //!< Path to the parameter mapping file on disk.
    time_t                mLastModified;          //!< File mtime at last load, used for change detection.
    MappingSearch_sptr    mMappingSearch;         //!< Forward index: parameter name → ParameterMapping records.
    MappingSearch_sptr    mMappingReverseSearch;  //!< Reverse index: parameter key → ParameterMapping records.
    ModificationLock      mModificationLock;      //!< Lock protecting index replacement during hot-reload.
    ThreadLock            mFileReadLock;          //!< Mutex serialising file I/O during reload.
};


using ParamMappingFile_vec = std::vector<ParameterMappingFile>;  //!< Ordered collection of ParameterMappingFile objects.


}  // namespace QueryServer
}  // namespace SmartMet
