#include "GridCoordinates.h"
#include <grid-files/common/Exception.h>
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
    mCoordinateType = T::CoordinateType::UNKNOWN;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GridCoordinates::GridCoordinates(GridCoordinates& gridCoordinates)
{
  try
  {
    mProjection =  gridCoordinates.mProjection;
    mColumns = gridCoordinates.mColumns;
    mRows = gridCoordinates.mRows;
    mProjectionAttributes = gridCoordinates.mProjectionAttributes;
    mCoordinateType = gridCoordinates.mCoordinateType;
    mCoordinateList = gridCoordinates.mCoordinateList;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





GridCoordinates::~GridCoordinates()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void GridCoordinates::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridCoordinates\n";
    stream << space(level) << "- mProjection     = " << mProjection << "\n";
    stream << space(level) << "- mProjectionAttributes\n";
    mProjectionAttributes.print(stream,level+2,optionFlags);
    stream << space(level) << "- mColumns        = " << mColumns << "\n";
    stream << space(level) << "- mRows           = " << mRows << "\n";
    stream << space(level) << "- mCoordinateType = " << (int)mCoordinateType << "\n";

    uint sz = (uint)mCoordinateList.size();
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}  // namespace SmartMet
