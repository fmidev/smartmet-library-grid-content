#pragma once

#include <macgyver/Exception.h>
#include <grid-files/common/AttributeList.h>
#include <grid-files/grid/Typedefs.h>

namespace SmartMet
{
namespace T
{


// ====================================================================================
/*! \brief A single-point value request and its result.
 *
 *  Identifies the source grid message and the query point, and on return
 *  carries the interpolated parameter value together with the service result code. */
// ====================================================================================

class ValueRecord
{
  public:
                    ValueRecord();
                    ValueRecord(ValueRecord& valueRecord);
    virtual         ~ValueRecord();

    virtual void    print(std::ostream& stream,uint level,uint optionFlags);

    T::FileId       mFileId;                 //!< File identifier of the source grid message.
    T::MessageIndex mMessageIndex;           //!< Message index within the file.
    CoordinateType  mCoordinateType;         //!< Coordinate system used for mX and mY.
    short           mAreaInterpolationMethod; //!< Spatial interpolation method code.
    double          mX;                      //!< X coordinate of the query point.
    double          mY;                      //!< Y coordinate of the query point.
    T::ParamValue   mValue;                  //!< Interpolated parameter value at the query point.
    int             mResult;                 //!< Service result code (DataServer::Result).
};



}
}  // namespace SmartMet
