#pragma once

#include "grid-files/common/Exception.h"
#include "grid-files/common/AttributeList.h"
#include "grid-files/grid/Typedefs.h"

namespace SmartMet
{
namespace T
{



class GridCoordinates
{
  public:
                       GridCoordinates();
                       GridCoordinates(GridCoordinates& gridCoordinates);
    virtual            ~GridCoordinates();

    virtual void       print(std::ostream& stream,uint level,uint optionFlags);

    uint               mProjection;
    uint               mColumns;
    uint               mRows;
    T::AttributeList   mProjectionAttributes;
    T::Coordinate_vec  mCoordinateList;
};



}
}  // namespace SmartMet
