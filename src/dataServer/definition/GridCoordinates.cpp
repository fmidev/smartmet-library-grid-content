#include "GridCoordinates.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{



GridCoordinates::GridCoordinates()
{
  try
  {

    mProjection = 0;
    mColumns = 0;
    mRows = 0;
    mReverseXDirection = false;
    mReverseYDirection = false;
    mCoordinateType = T::CoordinateTypeValue::UNKNOWN;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GridCoordinates::GridCoordinates(GridCoordinates& gridCoordinates)
{
  try
  {
    mProjection =  gridCoordinates.mProjection;
    mColumns = gridCoordinates.mColumns;
    mRows = gridCoordinates.mRows;
    mReverseXDirection = gridCoordinates.mReverseXDirection;
    mReverseYDirection = gridCoordinates.mReverseYDirection;
    mProjectionAttributes = gridCoordinates.mProjectionAttributes;
    mCoordinateType = gridCoordinates.mCoordinateType;
    mCoordinateList = gridCoordinates.mCoordinateList;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





GridCoordinates::~GridCoordinates()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void GridCoordinates::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridCoordinates\n";
    stream << space(level) << "- mProjection        = " << mProjection << "\n";
    stream << space(level) << "- mProjectionAttributes\n";
    mProjectionAttributes.print(stream,level+2,optionFlags);
    stream << space(level) << "- mColumns           = " << mColumns << "\n";
    stream << space(level) << "- mRows              = " << mRows << "\n";
    stream << space(level) << "- mReverseXDirection = " << (int)mReverseXDirection << "\n";
    stream << space(level) << "- mReverseYDirection = " << (int)mReverseYDirection << "\n";
    stream << space(level) << "- mCoordinateType    = " << mCoordinateType << "\n";

    uint sz = mCoordinateList.size();
    stream << space(level) << "- mCoordinateList (" << sz << ")\n";

    if (sz > 10)
      sz = 10;

    for (uint c=0; c<sz; c++)
    {
      stream << space(level+2) << "* " << mCoordinateList[c].x() << ", " << mCoordinateList[c].y() << "\n";
    }

  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}  // namespace SmartMet
