#pragma once

#include <macgyver/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

namespace SmartMet
{
namespace T
{



// ====================================================================================
/*! \brief Coordinate array for a single grid message.
 *
 *  Stores the projection type, grid dimensions, axis-reversal flags, and the
 *  flat list of (x,y) coordinate pairs that correspond to each grid point in
 *  row-major order. */
// ====================================================================================

class GridCoordinates
{
  public:
                       GridCoordinates();
                       GridCoordinates(GridCoordinates& gridCoordinates);
    virtual            ~GridCoordinates();

    virtual void       print(std::ostream& stream,uint level,uint optionFlags);

    uint               mProjection;           //!< Original projection identifier.
    T::AttributeList   mProjectionAttributes; //!< Projection-specific attribute list.
    uint               mColumns;              //!< Number of grid columns.
    uint               mRows;                 //!< Number of grid rows.
    bool               mReverseXDirection;    //!< True if the x-axis is stored in reverse order.
    bool               mReverseYDirection;    //!< True if the y-axis is stored in reverse order.
    T::CoordinateType  mCoordinateType;       //!< Coordinate type used in mCoordinateList.
    T::Coordinate_vec  mCoordinateList;       //!< Flat list of grid point coordinates.
};



}
}  // namespace SmartMet
