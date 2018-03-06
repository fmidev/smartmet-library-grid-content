#pragma once

#include "ValueRecord.h"
#include "contentServer/definition/ContentInfoList.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class ValueRecordList
{
  public:
                    ValueRecordList();
                    ValueRecordList(ValueRecordList& valueRecordList);
                    ValueRecordList(ContentInfoList& contentInfoList,CoordinateType coordinateType,T::AreaInterpolationMethod interpolationMethod,double x,double y);
     virtual        ~ValueRecordList();

     void           operator=(ValueRecordList& valueRecordList);

     void           addValueRecord(ValueRecord *valueRecord);
     void           clear();
     ValueRecord*   getValueRecordByIndex(uint index);
     uint           getLength();
     void           print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<ValueRecord*>  mList;
};


}
}
