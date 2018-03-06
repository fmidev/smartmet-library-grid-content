#pragma once

#include <grid-files/common/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

namespace SmartMet
{
namespace T
{


class ValueRecord
{
  public:
                                ValueRecord();
                                ValueRecord(ValueRecord& valueRecord);
    virtual                     ~ValueRecord();

    virtual void                print(std::ostream& stream,uint level,uint optionFlags);

    uint                        mFileId;
    uint                        mMessageIndex;
    CoordinateType              mCoordinateType;
    T::AreaInterpolationMethod  mAreaInterpolationMethod;
    double                      mX;
    double                      mY;
    T::ParamValue               mValue;
    int                         mResult;
};



}
}  // namespace SmartMet
