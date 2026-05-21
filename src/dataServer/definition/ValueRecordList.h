#pragma once

#include "ValueRecord.h"
#include "../../contentServer/definition/ContentInfoList.h"
#include <vector>


namespace SmartMet
{
namespace T
{


// ====================================================================================
/*! \brief An ordered collection of single-point value request/result records.
 *
 *  Used by getMultipleGridValues() to batch many single-point lookups across
 *  potentially different grid messages in a single service call.  The list
 *  owns the ValueRecord objects it contains. */
// ====================================================================================

class ValueRecordList
{
  public:
                      ValueRecordList();
                      ValueRecordList(ValueRecordList& valueRecordList);
                      ValueRecordList(ContentInfoList& contentInfoList,CoordinateType coordinateType,short interpolationMethod,double x,double y);
     virtual          ~ValueRecordList();

     ValueRecordList& operator=(ValueRecordList& valueRecordList);

     void             addValueRecord(ValueRecord *valueRecord);
     void             clear();
     ValueRecord*     getValueRecordByIndex(uint index) const;
     uint             getLength() const;
     void             print(std::ostream& stream,uint level,uint optionFlags) const;

  protected:

     std::vector<ValueRecord*>  mList;  //!< Owned list of value records.
};


}
}
