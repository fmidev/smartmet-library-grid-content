#pragma once

#include <string>
#include <map>
#include "grid-files/grid/Typedefs.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/ModificationLock.h"

namespace SmartMet
{
namespace DataServer
{

// ====================================================================================
/*!
  \brief This class is used for caching grid point values. This is usually needed in order
  to guarantee fast data processing.

*/
// ====================================================================================


struct CacheValueRec
{
  uint          fileId;
  //uint          messageIndex;
  //double        x;
  //double        y;
  T::ParamValue value;
  time_t        lastAccessTime;
};

typedef CacheValueRec* CacheValueRec_ptr;


class PointValueCache
{
  public:
                      PointValueCache();
    virtual           ~PointValueCache();

    void              addValue(uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue value);
    void              clear();
    bool              getValue(uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::ParamValue& value);
    void              deleteValue(uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod);
    void              deleteValuesByFileId(uint fileId);
    void              deleteValuesByAge(uint maxAge);
    void              setMaxSize(std::size_t maxSize);

  protected:

    std::size_t       getKey(uint fileId,uint messageIndex,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod);
    void              deleteValue(std::size_t key);

    std::size_t       mMaxSize;
    ModificationLock  mModificationLock;

    std::map<std::size_t,CacheValueRec_ptr> mValueList;
};


}
}
