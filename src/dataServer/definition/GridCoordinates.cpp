#include "GridCoordinates.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


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
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    mCoordinateList = gridCoordinates.mCoordinateList;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





GridCoordinates::~GridCoordinates()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void GridCoordinates::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridCoordinates\n";
    stream << space(level) << "- mProjection = " << mProjection << "\n";
    stream << space(level) << "- mColumns    = " << mColumns << "\n";
    stream << space(level) << "- mRows       = " << mRows << "\n";
    stream << space(level) << "- mProjectionAttributes\n";
    mProjectionAttributes.print(stream,level+2,optionFlags);

    uint sz = (uint)mCoordinateList.size();
    stream << space(level) << "- mCoordinateList (" << sz << ")\n";

    if (sz > 10)
      sz = 10;

    for (uint c=0; c<sz; c++)
    {
      stream << space(level+2) << "- " << mCoordinateList[c].x() << ", " << mCoordinateList[c].y() << "\n";
    }

  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




}
}  // namespace SmartMet
