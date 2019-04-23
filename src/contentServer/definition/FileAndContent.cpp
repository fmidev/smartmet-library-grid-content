#include "FileAndContent.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


FileAndContent::FileAndContent()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileAndContent::FileAndContent(const FileAndContent& fileAndContent)
{
  try
  {
    mFileInfo = fileAndContent.mFileInfo;
    mContentInfoList = fileAndContent.mContentInfoList;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileAndContent::~FileAndContent()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





FileAndContent* FileAndContent::duplicate()
{
  try
  {
    return new FileAndContent(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
