#include "ServerInterface.h"
#include "../convert/Converter.h"

#include <macgyver/Exception.h>
#include <grid-files/common/ShowFunction.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace ContentServer
{
namespace Corba
{



ServerInterface::ServerInterface()
{
  FUNCTION_TRACE
  try
  {
    mService = nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ServerInterface::~ServerInterface()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void ServerInterface::init(ContentServer::ServiceInterface *service)
{
  FUNCTION_TRACE
  try
  {
    mService = service;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





::CORBA::Long ServerInterface::clear(::CORBA::LongLong sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->clear(sessionId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::reload(::CORBA::LongLong sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->reload(sessionId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addProducerInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ProducerInfo sProducerInfo;
    ContentServer::Corba::Converter::convert(producerInfo,sProducerInfo);

    int result = mService->addProducerInfo(sessionId,sProducerInfo);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfo,producerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteProducerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteProducerInfoById(sessionId,producerId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteProducerInfoByName(::CORBA::LongLong sessionId, const char* producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteProducerInfoByName(sessionId,producerName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteProducerInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteProducerInfoListBySourceId(sessionId,sourceId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, SmartMet::ContentServer::Corba::CorbaProducerInfo_out producerInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo sProducerInfo;
    ContentServer::Corba::CorbaProducerInfo *corbaProducerInfo = new ContentServer::Corba::CorbaProducerInfo();
    producerInfo = corbaProducerInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoById(sessionId,producerId,sProducerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfo,*corbaProducerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoByName(::CORBA::LongLong sessionId, const char* producerName, SmartMet::ContentServer::Corba::CorbaProducerInfo_out producerInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo sProducerInfo;
    ContentServer::Corba::CorbaProducerInfo *corbaProducerInfo = new ContentServer::Corba::CorbaProducerInfo();
    producerInfo = corbaProducerInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoByName(sessionId,producerName,sProducerInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfo,*corbaProducerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    ContentServer::Corba::CorbaProducerInfoList *corbaProducerInfoList = new ContentServer::Corba::CorbaProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoList(sessionId,sProducerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoListByParameter(::CORBA::LongLong sessionId, ::CORBA::Octet parameterKeyType, const char* parameterKey, SmartMet::ContentServer::Corba::CorbaProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    ContentServer::Corba::CorbaProducerInfoList *corbaProducerInfoList = new ContentServer::Corba::CorbaProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,sProducerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId, SmartMet::ContentServer::Corba::CorbaProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    ContentServer::Corba::CorbaProducerInfoList *corbaProducerInfoList = new ContentServer::Corba::CorbaProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoListBySourceId(sessionId,sourceId,sProducerInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getProducerInfoCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerNameAndGeometryList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaStringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    ContentServer::Corba::CorbaStringList *corbaList = new ContentServer::Corba::CorbaStringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerNameAndGeometryList(sessionId,sList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerParameterList(::CORBA::LongLong sessionId, ::CORBA::Octet sourceParameterKeyType, ::CORBA::Octet targetParameterKeyType, SmartMet::ContentServer::Corba::CorbaStringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    ContentServer::Corba::CorbaStringList *corbaList = new ContentServer::Corba::CorbaStringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,sList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getProducerParameterListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId,::CORBA::Octet sourceParameterKeyType, ::CORBA::Octet targetParameterKeyType, SmartMet::ContentServer::Corba::CorbaStringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    ContentServer::Corba::CorbaStringList *corbaList = new ContentServer::Corba::CorbaStringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,sList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addGenerationInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaGenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GenerationInfo sGenerationInfo;
    ContentServer::Corba::Converter::convert(generationInfo,sGenerationInfo);

    int result = mService->addGenerationInfo(sessionId,sGenerationInfo);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfo,generationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteGenerationInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGenerationInfoById(sessionId,generationId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteGenerationInfoByName(::CORBA::LongLong sessionId, const char* generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGenerationInfoByName(sessionId,generationName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteGenerationInfoListByIdList(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaULongList& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::set<uint> sGenerationIdList;
    ContentServer::Corba::Converter::convert(generationIdList,sGenerationIdList);

    int result = mService->deleteGenerationInfoListByIdList(sessionId,sGenerationIdList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteGenerationInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGenerationInfoListByProducerId(sessionId,producerId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteGenerationInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGenerationInfoListByProducerName(sessionId,producerName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::deleteGenerationInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGenerationInfoListBySourceId(sessionId,sourceId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationIdGeometryIdAndForecastTimeList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaStringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    ContentServer::Corba::CorbaStringList *corbaList = new ContentServer::Corba::CorbaStringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationIdGeometryIdAndForecastTimeList(sessionId,sList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    ContentServer::Corba::CorbaGenerationInfo *corbaGenerationInfo = new ContentServer::Corba::CorbaGenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoById(sessionId,generationId,sGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoByName(::CORBA::LongLong sessionId, const char* generationName, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    ContentServer::Corba::CorbaGenerationInfo *corbaGenerationInfo = new ContentServer::Corba::CorbaGenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoByName(sessionId,generationName,sGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    ContentServer::Corba::CorbaGenerationInfoList *corbaGenerationInfoList = new ContentServer::Corba::CorbaGenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoList(sessionId,sGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoListByGeometryId(::CORBA::LongLong sessionId, ::CORBA::Long geometryId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    ContentServer::Corba::CorbaGenerationInfoList *corbaGenerationInfoList = new ContentServer::Corba::CorbaGenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByGeometryId(sessionId,geometryId,sGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    ContentServer::Corba::CorbaGenerationInfoList *corbaGenerationInfoList = new ContentServer::Corba::CorbaGenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByProducerId(sessionId,producerId,sGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    ContentServer::Corba::CorbaGenerationInfoList *corbaGenerationInfoList = new ContentServer::Corba::CorbaGenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByProducerName(sessionId,producerName,sGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId, SmartMet::ContentServer::Corba::CorbaGenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    ContentServer::Corba::CorbaGenerationInfoList *corbaGenerationInfoList = new ContentServer::Corba::CorbaGenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListBySourceId(sessionId,sourceId,sGenerationInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getLastGenerationInfoByProducerIdAndStatus(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::Octet generationStatus, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    ContentServer::Corba::CorbaGenerationInfo *corbaGenerationInfo = new ContentServer::Corba::CorbaGenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,sGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getLastGenerationInfoByProducerNameAndStatus(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::Octet generationStatus, SmartMet::ContentServer::Corba::CorbaGenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    ContentServer::Corba::CorbaGenerationInfo *corbaGenerationInfo = new ContentServer::Corba::CorbaGenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,sGenerationInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getGenerationInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getGenerationInfoCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::setGenerationInfoStatusById(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet status)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->setGenerationInfoStatusById(sessionId,generationId,status);
    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::setGenerationInfoStatusByName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::Octet status)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->setGenerationInfoStatusByName(sessionId,generationName,status);
    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addFileInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaFileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::FileInfo sFileInfo;
    ContentServer::Corba::Converter::convert(fileInfo,sFileInfo);

    int result = mService->addFileInfo(sessionId,sFileInfo);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfo,fileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addFileInfoWithContentList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaFileInfo& fileInfo, SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::FileInfo sFileInfo;
    T::ContentInfoList sContentInfoList;

    ContentServer::Corba::Converter::convert(fileInfo,sFileInfo);
    ContentServer::Corba::Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->addFileInfoWithContentList(sessionId,sFileInfo,sContentInfoList);
    if (result == 0)
    {
      ContentServer::Corba::Converter::convert(sFileInfo,fileInfo);
      ContentServer::Corba::Converter::convert(sContentInfoList,contentInfoList);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addFileInfoListWithContent(::CORBA::LongLong sessionId, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaFileContentList& fileContentList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::vector<T::FileAndContent> sFileContentList;

    ContentServer::Corba::Converter::convert(fileContentList,sFileContentList);

    int result = mService->addFileInfoListWithContent(sessionId,requestFlags,sFileContentList);
    if (result == 0)
    {
      ContentServer::Corba::Converter::convert(sFileContentList,fileContentList);
    }

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoById(sessionId,fileId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoByName(::CORBA::LongLong sessionId, const char* filename)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoByName(sessionId,filename);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByGroupFlags(sessionId,groupFlags);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByProducerId(sessionId,producerId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByProducerName(sessionId,producerName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByGenerationId(::CORBA::LongLong sessionId,::CORBA::ULong generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByGenerationId(sessionId,generationId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByGenerationIdAndForecastTime(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Long geometryId, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, const char* forecastTime)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByGenerationIdAndForecastTime(sessionId,generationId,geometryId,forecastType,forecastNumber,forecastTime);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByForecastTimeList(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaForecastTimeList& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::vector<T::ForecastTime> sForecastTimeList;

    ContentServer::Corba::Converter::convert(forecastTimeList,sForecastTimeList);

    int result = mService->deleteFileInfoListByForecastTimeList(sessionId,sForecastTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::deleteFileInfoListByGenerationName(::CORBA::LongLong sessionId, const char* generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListByGenerationName(sessionId,generationName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteFileInfoListBySourceId(sessionId,sourceId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteFileInfoListByFileIdList(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaULongList& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::set<uint> sFileIdList;
    ContentServer::Corba::Converter::convert(fileIdList,sFileIdList);

    int result = mService->deleteFileInfoListByFileIdList(sessionId,sFileIdList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoById(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, SmartMet::ContentServer::Corba::CorbaFileInfo_out fileInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfo sFileInfo;
    ContentServer::Corba::CorbaFileInfo *corbaFileInfo = new ContentServer::Corba::CorbaFileInfo();
    fileInfo = corbaFileInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoById(sessionId,fileId,sFileInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfo,*corbaFileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoByName(::CORBA::LongLong sessionId, const char* filename, SmartMet::ContentServer::Corba::CorbaFileInfo_out fileInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfo sFileInfo;
    ContentServer::Corba::CorbaFileInfo *corbaFileInfo = new ContentServer::Corba::CorbaFileInfo();
    fileInfo = corbaFileInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoByName(sessionId,filename,sFileInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfo,*corbaFileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoList(::CORBA::LongLong sessionId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoList(sessionId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoListByFileIdList(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaULongList& fileIdList, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    std::vector<uint> sFileIdList;
    ContentServer::Corba::Converter::convert(fileIdList,sFileIdList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByFileIdList(sessionId,sFileIdList,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}







::CORBA::Long ServerInterface::getFileInfoListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoListByProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::getFileInfoListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId,::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoListByGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByGroupFlags(sessionId,groupFlags,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId, ::CORBA::ULong startFileId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaFileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    ContentServer::Corba::CorbaFileInfoList *corbaFileInfoList = new ContentServer::Corba::CorbaFileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getFileInfoCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoCountByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getFileInfoCountByProducerId(sessionId,producerId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoCountByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getFileInfoCountByGenerationId(sessionId,generationId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getFileInfoCountBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getFileInfoCountBySourceId(sessionId,sourceId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addEventInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaEventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::EventInfo sEventInfo;
    ContentServer::Corba::Converter::convert(eventInfo,sEventInfo);

    int result = mService->addEventInfo(sessionId,sEventInfo);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sEventInfo,eventInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getLastEventInfo(::CORBA::LongLong sessionId, ::CORBA::ULong requestingServerId, SmartMet::ContentServer::Corba::CorbaEventInfo_out eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::EventInfo sEventInfo;
    ContentServer::Corba::CorbaEventInfo *corbaEventInfo = new ContentServer::Corba::CorbaEventInfo();
    eventInfo = corbaEventInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastEventInfo(sessionId,requestingServerId,sEventInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sEventInfo,*corbaEventInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getEventInfoList(::CORBA::LongLong sessionId, ::CORBA::ULong requestingServerId, ::CORBA::ULongLong startEventId, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaEventInfoList_out eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::EventInfoList sEventInfoList;
    ContentServer::Corba::CorbaEventInfoList *corbaEventInfoList = new ContentServer::Corba::CorbaEventInfoList();
    eventInfoList = corbaEventInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,sEventInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sEventInfoList,*corbaEventInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getEventInfoCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getEventInfoCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addContentInfo(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ContentInfo sContentInfo;
    ContentServer::Corba::Converter::convert(contentInfo,sContentInfo);

    int result = mService->addContentInfo(sessionId,sContentInfo);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfo,contentInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::addContentList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::Converter::convert(contentInfoList,sContentInfoList);

    //sContentInfoList.print(std::cout,0,0);

    int result = mService->addContentList(sessionId,sContentInfoList);
    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,contentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentInfo(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentInfo(sessionId,fileId,messageIndex);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong fileId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByFileId(sessionId,fileId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByFileName(::CORBA::LongLong sessionId, const char* filename)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByFileName(sessionId,filename);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




::CORBA::Long ServerInterface::deleteContentListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByGroupFlags(sessionId,groupFlags);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByProducerId(sessionId,producerId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByProducerName(::CORBA::LongLong sessionId, const char* producerName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByProducerName(sessionId,producerName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByGenerationId(sessionId,generationId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListByGenerationName(::CORBA::LongLong sessionId, const char* generationName)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListByGenerationName(sessionId,generationName);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteContentListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteContentListBySourceId(sessionId,sourceId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentInfo(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, ::CORBA::ULong messageIndex, SmartMet::ContentServer::Corba::CorbaContentInfo_out contentInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfo sContentInfo;
    ContentServer::Corba::CorbaContentInfo *corbaContentInfo = new ContentServer::Corba::CorbaContentInfo();
    contentInfo = corbaContentInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentInfo(sessionId,fileId,messageIndex,sContentInfo);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfo,*corbaContentInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentList(::CORBA::LongLong sessionId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentList(sessionId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByFileId(::CORBA::LongLong sessionId, ::CORBA::ULong fileId, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileId(sessionId,fileId,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByFileIdList(::CORBA::LongLong sessionId, const SmartMet::ContentServer::Corba::CorbaULongList& fileIdList, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    std::vector<uint> sFileIdList;
    ContentServer::Corba::Converter::convert(fileIdList,sFileIdList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileIdList(sessionId,sFileIdList,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByFileName(::CORBA::LongLong sessionId, const char* filename, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileName(sessionId,filename,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByGroupFlags(::CORBA::LongLong sessionId, ::CORBA::ULong groupFlags, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGroupFlags(sessionId,groupFlags,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByGenerationIdAndTimeRange(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, const char* startTime, const char* endTime, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByGenerationNameAndTimeRange(::CORBA::LongLong sessionId, const char* generationName, const char* startTime, const char* endTime, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListBySourceId(::CORBA::LongLong sessionId, ::CORBA::ULong sourceId, ::CORBA::ULong startFileId, ::CORBA::ULong startMessageIndex, ::CORBA::ULong maxRecords, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameter(::CORBA::LongLong sessionId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long minLevel, ::CORBA::Long maxLevel, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameter(sessionId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameterAndGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long minLevel, ::CORBA::Long maxLevel, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameterAndGenerationName(::CORBA::LongLong sessionId, const char* generationName, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long minLevel, ::CORBA::Long maxLevel, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameterAndProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long minLevel, ::CORBA::Long maxLevel, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameterAndProducerName(::CORBA::LongLong sessionId, const char* producerName, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long minLevel, ::CORBA::Long maxLevel, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* startTime, const char* endTime, ::CORBA::ULong requestFlags, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListByParameterGenerationIdAndForecastTime(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet parameterKeyType, const char* parameterKey, ::CORBA::Octet parameterLevelIdType, ::CORBA::ULong parameterLevelId, ::CORBA::Long level, ::CORBA::Short forecastType, ::CORBA::Short forecastNumber, ::CORBA::Long geometryId, const char* forecastTime, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,parameterLevelIdType,parameterLevelId,level,forecastType,forecastNumber,geometryId,forecastTime,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentListOfInvalidIntegrity(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentInfoList = new ContentServer::Corba::CorbaContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListOfInvalidIntegrity(sessionId,sContentInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentGeometryIdListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, SmartMet::ContentServer::Corba::CorbaLongList_out geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::set<T::GeometryId> sGeometryIdList;
    ContentServer::Corba::CorbaLongList *corbaGeometryIdList = new ContentServer::Corba::CorbaLongList();
    geometryIdList = corbaGeometryIdList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentGeometryIdListByGenerationId(sessionId,generationId,sGeometryIdList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sGeometryIdList,*corbaGeometryIdList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentParamListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, SmartMet::ContentServer::Corba::CorbaContentInfoList_out contentParamList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentParamList;
    ContentServer::Corba::CorbaContentInfoList *corbaContentParamList = new ContentServer::Corba::CorbaContentInfoList();
    contentParamList = corbaContentParamList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamListByGenerationId(sessionId,generationId,sContentParamList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentParamList,*corbaContentParamList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentParamKeyListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Octet parameterKeyType, SmartMet::ContentServer::Corba::CorbaStringList_out paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sParamKeyList;
    ContentServer::Corba::CorbaStringList *corbaParamKeyList = new ContentServer::Corba::CorbaStringList();
    paramKeyList = corbaParamKeyList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,sParamKeyList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sParamKeyList,*corbaParamKeyList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentTimeListByGenerationId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, SmartMet::ContentServer::Corba::CorbaStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    ContentServer::Corba::CorbaStringList *corbaContentTimeList = new ContentServer::Corba::CorbaStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByGenerationId(sessionId,generationId,sContentTimeList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentTimeListByGenerationAndGeometryId(::CORBA::LongLong sessionId, ::CORBA::ULong generationId, ::CORBA::Long geometryId, SmartMet::ContentServer::Corba::CorbaStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    ContentServer::Corba::CorbaStringList *corbaContentTimeList = new ContentServer::Corba::CorbaStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,sContentTimeList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentTimeListByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, SmartMet::ContentServer::Corba::CorbaStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    ContentServer::Corba::CorbaStringList *corbaContentTimeList = new ContentServer::Corba::CorbaStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByProducerId(sessionId,producerId,sContentTimeList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getContentCount(::CORBA::LongLong sessionId, ::CORBA::ULong& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getContentCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<::CORBA::ULong>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getHashByProducerId(::CORBA::LongLong sessionId, ::CORBA::ULong producerId, ::CORBA::ULongLong& hash)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    hash = 0;
    ulonglong sHash = 0;

    int result = mService->getHashByProducerId(sessionId,producerId,sHash);

    if (result == 0)
      hash = static_cast<::CORBA::ULongLong>(sHash);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::getLevelInfoList(::CORBA::LongLong sessionId, SmartMet::ContentServer::Corba::CorbaLevelInfoList_out levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::LevelInfoList sLevelInfoList;
    ContentServer::Corba::CorbaLevelInfoList *corbaLevelInfoList = new ContentServer::Corba::CorbaLevelInfoList();
    levelInfoList = corbaLevelInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLevelInfoList(sessionId,sLevelInfoList);

    if (result == 0)
      ContentServer::Corba::Converter::convert(sLevelInfoList,*corbaLevelInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::deleteVirtualContent(::CORBA::LongLong sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->deleteVirtualContent(sessionId);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





::CORBA::Long ServerInterface::updateVirtualContent(::CORBA::LongLong sessionId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->updateVirtualContent(sessionId);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}

}
}
}
