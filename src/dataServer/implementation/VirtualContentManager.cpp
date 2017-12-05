#include "VirtualContentManager.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{


VirtualContentManager::VirtualContentManager()
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





VirtualContentManager::~VirtualContentManager()
{
  FUNCTION_TRACE
  try
  {
    for (auto it = mFactoryList.begin(); it != mFactoryList.end(); ++it)
      delete *it;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentManager::init()
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





void VirtualContentManager::addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    //printf("**** ADD FILE ***\n");
    //fileInfo.print(std::cout,0,0);
    for (auto it = mFactoryList.begin(); it != mFactoryList.end(); ++it)
    {
      (*it)->addFile(producerInfo,generationInfo,fileInfo,contentInfoList,gridFileMap);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentManager::addVirtualContentFactory(VirtualContentFactory_ptr factory)
{
  FUNCTION_TRACE
  try
  {
    mFactoryList.push_back(factory);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





}
}
