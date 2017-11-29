#include "VirtualContentFactory.h"
#include "VirtualMessage.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{


VirtualContentFactory::VirtualContentFactory()
{
  FUNCTION_TRACE
  try
  {
    mContentServer = NULL;
    mGridFileManager = NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





VirtualContentFactory::~VirtualContentFactory()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::setContentServer(ContentServer::ServiceInterface* contentServer)
{
  FUNCTION_TRACE
  try
  {
    mContentServer = contentServer;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::setGridFileManager(GridFileManager* GridFileManager)
{
  FUNCTION_TRACE
  try
  {
    mGridFileManager = GridFileManager;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::setLuaFileCollection(Lua::LuaFileCollection *luaFileCollection)
{
  FUNCTION_TRACE
  try
  {
    mLuaFileCollection = luaFileCollection;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::setFunctionCollection(Functions::FunctionCollection *functionCollection)
{
  FUNCTION_TRACE
  try
  {
    mFunctionCollection = functionCollection;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      addContent(fileInfo,*contentInfo,gridFileMap);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory::addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}



}
}