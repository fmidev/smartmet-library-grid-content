#pragma once

#include "grid-files/grid/Typedefs.h"

namespace SmartMet
{
namespace T
{


class GridValue
{
  public:
                  GridValue();
                  GridValue(GridValue& gridValue);
                  GridValue(double x,double y, T::ParamValue value);
    virtual       ~GridValue();

    void          print(std::ostream& stream,uint level,uint optionFlags);

    double        mX;
    double        mY;
    T::ParamValue mValue;
};



}
}  // namespace SmartMet
