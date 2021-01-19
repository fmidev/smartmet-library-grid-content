#include "VirtualContentManager.h"
#include <grid-files/common/ShowFunction.h>
#include <macgyver/Exception.h>
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualContentManager::addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    for (auto it = mFactoryList.begin(); it != mFactoryList.end(); ++it)
    {
      (*it)->addFile(producerInfo,generationInfo,fileInfo,contentInfoList,gridFileMap);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualContentManager::addVirtualContentFactory(VirtualContentFactory_ptr factory)
{
  FUNCTION_TRACE
  try
  {
    mFactoryList.emplace_back(factory);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





}
}
