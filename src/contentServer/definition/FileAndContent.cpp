#include "FileAndContent.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for FileAndContent. */

FileAndContent::FileAndContent()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for FileAndContent. */

FileAndContent::FileAndContent(const FileAndContent& fileAndContent)
{
  try
  {
    mFileInfo = fileAndContent.mFileInfo;
    mContentInfoList = fileAndContent.mContentInfoList;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for FileAndContent. */

FileAndContent::~FileAndContent()
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





/*! \brief Return a deep copy of this record. */

FileAndContent* FileAndContent::duplicate()
{
  try
  {
    return new FileAndContent(*this);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Print the record contents to the given stream. */

void FileAndContent::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "FileInfo\n";
    mFileInfo.print(stream,level+1,optionFlags);
    mContentInfoList.print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
