#pragma once

#include <grid-files/common/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

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

    uint               mProjection;           // Original projection
    T::AttributeList   mProjectionAttributes; // Original projection attributes
    uint               mColumns;              // Grid columns
    uint               mRows;                 // Grid rows
    T::CoordinateType  mCoordinateType;       // Coordinate type used in the mCoordinateList
    T::Coordinate_vec  mCoordinateList;       // Grid coordinates
};



}
}  // namespace SmartMet
