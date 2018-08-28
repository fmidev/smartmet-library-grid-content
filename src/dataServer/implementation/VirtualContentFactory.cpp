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
    mContentServer = nullptr;
    mGridFileManager = nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualContentFactory::addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      addContent(producerInfo,generationInfo,fileInfo,*contentInfo,gridFileMap);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void VirtualContentFactory::addContent(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}



}
}
