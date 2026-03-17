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





SmartMet::C::Result ServerInterface::ServerInterface::clear(SmartMet::C::SessionId sessionId)
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





SmartMet::C::Result ServerInterface::ServerInterface::reload(SmartMet::C::SessionId sessionId)
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





SmartMet::C::Result ServerInterface::ServerInterface::getContentChangeTime(SmartMet::C::SessionId sessionId, SmartMet::C::Timestamp& changeTime)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    changeTime = 0;
    time_t sTime = 0;

    int result = mService->getContentChangeTime(sessionId,sTime);

    if (result == 0)
      changeTime = static_cast<UInt32>(sTime);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::ServerInterface::addProducerInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ProducerInfo sProducerInfo;
    Converter::convert(producerInfo,sProducerInfo);

    int result = mService->addProducerInfo(sessionId,sProducerInfo);
    if (result == 0)
      Converter::convert(sProducerInfo,producerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setProducerInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ProducerInfo& producerInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ProducerInfo sProducerInfo;
    Converter::convert(producerInfo,sProducerInfo);

    return mService->setProducerInfo(sessionId,sProducerInfo);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}







SmartMet::C::Result ServerInterface::ServerInterface::deleteProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId)
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





SmartMet::C::Result ServerInterface::deleteProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName)
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





SmartMet::C::Result ServerInterface::deleteProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId)
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





SmartMet::C::Result ServerInterface::getProducerInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ProducerInfo_out producerInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo sProducerInfo;
    C::ProducerInfo *corbaProducerInfo = new C::ProducerInfo();
    producerInfo = corbaProducerInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoById(sessionId,producerId,sProducerInfo);

    if (result == 0)
      Converter::convert(sProducerInfo,*corbaProducerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerInfoByName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ProducerInfo_out producerInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfo sProducerInfo;
    C::ProducerInfo *corbaProducerInfo = new C::ProducerInfo();
    producerInfo = corbaProducerInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoByName(sessionId,producerName,sProducerInfo);

    if (result == 0)
      Converter::convert(sProducerInfo,*corbaProducerInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    C::ProducerInfoList *corbaProducerInfoList = new C::ProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoList(sessionId,sProducerInfoList);

    if (result == 0)
      Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerInfoListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::ProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    C::ProducerInfoList *corbaProducerInfoList = new C::ProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoListByParameter(sessionId,parameterKeyType,parameterKey,sProducerInfoList);

    if (result == 0)
      Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::ProducerInfoList_out producerInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ProducerInfoList sProducerInfoList;
    C::ProducerInfoList *corbaProducerInfoList = new C::ProducerInfoList();
    producerInfoList = corbaProducerInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerInfoListBySourceId(sessionId,sourceId,sProducerInfoList);

    if (result == 0)
      Converter::convert(sProducerInfoList,*corbaProducerInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerNameAndGeometryList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    C::StringList *corbaList = new C::StringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerNameAndGeometryList(sessionId,sList);

    if (result == 0)
      Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerParameterList(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    C::StringList *corbaList = new C::StringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerParameterList(sessionId,sourceParameterKeyType,targetParameterKeyType,sList);

    if (result == 0)
      Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getProducerParameterListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType sourceParameterKeyType, SmartMet::C::ParameterKeyType targetParameterKeyType, SmartMet::C::StringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    C::StringList *corbaList = new C::StringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getProducerParameterListByProducerId(sessionId,producerId,sourceParameterKeyType,targetParameterKeyType,sList);

    if (result == 0)
      Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::addGenerationInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GenerationInfo sGenerationInfo;
    Converter::convert(generationInfo,sGenerationInfo);

    int result = mService->addGenerationInfo(sessionId,sGenerationInfo);
    if (result == 0)
      Converter::convert(sGenerationInfo,generationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setGenerationInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationInfo& generationInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GenerationInfo sGenerationInfo;
    Converter::convert(generationInfo,sGenerationInfo);

    return mService->setGenerationInfo(sessionId,sGenerationInfo);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId)
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





SmartMet::C::Result ServerInterface::deleteGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName)
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





SmartMet::C::Result ServerInterface::deleteGenerationInfoListByIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::GenerationIdList& generationIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::set<T::GenerationId> sGenerationIdList;
    Converter::convert(generationIdList,sGenerationIdList);

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





SmartMet::C::Result ServerInterface::deleteGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId)
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





SmartMet::C::Result ServerInterface::deleteGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName)
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




SmartMet::C::Result ServerInterface::deleteGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId)
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





SmartMet::C::Result ServerInterface::getGenerationIdGeometryIdAndForecastTimeList(SmartMet::C::SessionId sessionId, SmartMet::C::StringList_out list)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sList;
    C::StringList *corbaList = new C::StringList();
    list = corbaList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationIdGeometryIdAndForecastTimeList(sessionId,sList);

    if (result == 0)
      Converter::convert(sList,*corbaList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getGenerationInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    C::GenerationInfo *corbaGenerationInfo = new C::GenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoById(sessionId,generationId,sGenerationInfo);

    if (result == 0)
      Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::GenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    C::GenerationInfo *corbaGenerationInfo = new C::GenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoByName(sessionId,generationName,sGenerationInfo);

    if (result == 0)
      Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    C::GenerationInfoList *corbaGenerationInfoList = new C::GenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoList(sessionId,sGenerationInfoList);

    if (result == 0)
      Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoListByGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryId geometryId, SmartMet::C::GenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    C::GenerationInfoList *corbaGenerationInfoList = new C::GenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByGeometryId(sessionId,geometryId,sGenerationInfoList);

    if (result == 0)
      Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    C::GenerationInfoList *corbaGenerationInfoList = new C::GenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByProducerId(sessionId,producerId,sGenerationInfoList);

    if (result == 0)
      Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::GenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    C::GenerationInfoList *corbaGenerationInfoList = new C::GenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListByProducerName(sessionId,producerName,sGenerationInfoList);

    if (result == 0)
      Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GenerationInfoList_out generationInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfoList sGenerationInfoList;
    C::GenerationInfoList *corbaGenerationInfoList = new C::GenerationInfoList();
    generationInfoList = corbaGenerationInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGenerationInfoListBySourceId(sessionId,sourceId,sGenerationInfoList);

    if (result == 0)
      Converter::convert(sGenerationInfoList,*corbaGenerationInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getLastGenerationInfoByProducerIdAndStatus(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    C::GenerationInfo *corbaGenerationInfo = new C::GenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastGenerationInfoByProducerIdAndStatus(sessionId,producerId,generationStatus,sGenerationInfo);

    if (result == 0)
      Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getLastGenerationInfoByProducerNameAndStatus(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::Status generationStatus, SmartMet::C::GenerationInfo_out generationInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GenerationInfo sGenerationInfo;
    C::GenerationInfo *corbaGenerationInfo = new C::GenerationInfo();
    generationInfo = corbaGenerationInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastGenerationInfoByProducerNameAndStatus(sessionId,producerName,generationStatus,sGenerationInfo);

    if (result == 0)
      Converter::convert(sGenerationInfo,*corbaGenerationInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGenerationInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::setGenerationInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Status status)
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





SmartMet::C::Result ServerInterface::setGenerationInfoStatusByName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Status status)
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





SmartMet::C::Result ServerInterface::addGeometryInfo(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GeometryInfo sGeometryInfo;
    Converter::convert(geometryInfo,sGeometryInfo);

    int result = mService->addGeometryInfo(sessionId,sGeometryInfo);
    if (result == 0)
      Converter::convert(sGeometryInfo,geometryInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGeometryInfoById(sessionId,generationId,geometryId,levelId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGeometryInfoListByGenerationId(sessionId,generationId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGeometryInfoListByProducerId(sessionId,producerId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->deleteGeometryInfoListBySourceId(sessionId,sourceId);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGeometryInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfo sGeometryInfo;
//    C::GeometryInfo *corbaGeometryInfo = new C::GeometryInfo();
//    geometryInfo = corbaGeometryInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGeometryInfoById(sessionId,generationId,geometryId,levelId,sGeometryInfo);

    if (result == 0)
      Converter::convert(sGeometryInfo,geometryInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGeometryInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::GeometryInfoList_out geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfoList sGeometryInfoList;
    C::GeometryInfoList *corbaGeometryInfoList = new C::GeometryInfoList();
    geometryInfoList = corbaGeometryInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGeometryInfoList(sessionId,sGeometryInfoList);

    if (result == 0)
      Converter::convert(sGeometryInfoList,*corbaGeometryInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getGeometryInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryInfoList_out geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfoList sGeometryInfoList;
    C::GeometryInfoList *corbaGeometryInfoList = new C::GeometryInfoList();
    geometryInfoList = corbaGeometryInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGeometryInfoListByGenerationId(sessionId,generationId,sGeometryInfoList);

    if (result == 0)
      Converter::convert(sGeometryInfoList,*corbaGeometryInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getGeometryInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GeometryInfoList_out geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfoList sGeometryInfoList;
    C::GeometryInfoList *corbaGeometryInfoList = new C::GeometryInfoList();
    geometryInfoList = corbaGeometryInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGeometryInfoListByProducerId(sessionId,producerId,sGeometryInfoList);

    if (result == 0)
      Converter::convert(sGeometryInfoList,*corbaGeometryInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getGeometryInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::GeometryInfoList_out geometryInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::GeometryInfoList sGeometryInfoList;
    C::GeometryInfoList *corbaGeometryInfoList = new C::GeometryInfoList();
    geometryInfoList = corbaGeometryInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getGeometryInfoListBySourceId(sessionId,sourceId,sGeometryInfoList);

    if (result == 0)
      Converter::convert(sGeometryInfoList,*corbaGeometryInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getGeometryInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    count = 0;
    uint sCount = 0;

    int result = mService->getGeometryInfoCount(sessionId,sCount);

    if (result == 0)
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setGeometryInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::GeometryInfo& geometryInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::GeometryInfo sGeometryInfo;
    Converter::convert(geometryInfo,sGeometryInfo);

    return mService->setGeometryInfo(sessionId,sGeometryInfo);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setGeometryInfoStatusById(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::Status status)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    return mService->setGeometryInfoStatusById(sessionId,generationId,geometryId,levelId,status);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::addFileInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::FileInfo sFileInfo;
    Converter::convert(fileInfo,sFileInfo);

    int result = mService->addFileInfo(sessionId,sFileInfo);
    if (result == 0)
      Converter::convert(sFileInfo,fileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setFileInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::FileInfo& fileInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::FileInfo sFileInfo;
    Converter::convert(fileInfo,sFileInfo);

    return mService->setFileInfo(sessionId,sFileInfo);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::addFileInfoWithContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileInfo& fileInfo, SmartMet::C::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::FileInfo sFileInfo;
    T::ContentInfoList sContentInfoList;

    Converter::convert(fileInfo,sFileInfo);
    Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->addFileInfoWithContentList(sessionId,sFileInfo,sContentInfoList);
    if (result == 0)
    {
      Converter::convert(sFileInfo,fileInfo);
      Converter::convert(sContentInfoList,contentInfoList);
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





SmartMet::C::Result ServerInterface::addFileInfoListWithContent(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestFlags, SmartMet::C::FileContentList& fileContentList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::vector<T::FileAndContent> sFileContentList;

    Converter::convert(fileContentList,sFileContentList);

    int result = mService->addFileInfoListWithContent(sessionId,requestFlags,sFileContentList);
    if (result == 0)
    {
      Converter::convert(sFileContentList,fileContentList);
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





SmartMet::C::Result ServerInterface::deleteFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId)
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





SmartMet::C::Result ServerInterface::deleteFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId)
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


SmartMet::C::Result ServerInterface::deleteFileInfoListByGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::Timestamp forecastTime)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListByForecastTimeList(SmartMet::C::SessionId sessionId, const SmartMet::C::ForecastTimeInfoList& forecastTimeList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::vector<T::ForecastTime> sForecastTimeList;

    Converter::convert(forecastTimeList,sForecastTimeList);

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




SmartMet::C::Result ServerInterface::deleteFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId)
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





SmartMet::C::Result ServerInterface::deleteFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    std::set<T::FileId> sFileIdList;
    Converter::convert(fileIdList,sFileIdList);

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





SmartMet::C::Result ServerInterface::getFileInfoById(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::FileInfo_out fileInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfo sFileInfo;
    C::FileInfo *corbaFileInfo = new C::FileInfo();
    fileInfo = corbaFileInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoById(sessionId,fileId,sFileInfo);

    if (result == 0)
      Converter::convert(sFileInfo,*corbaFileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoByName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::FileInfo_out fileInfo)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfo sFileInfo;
    C::FileInfo *corbaFileInfo = new C::FileInfo();
    fileInfo = corbaFileInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoByName(sessionId,filename,sFileInfo);

    if (result == 0)
      Converter::convert(sFileInfo,*corbaFileInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoList(sessionId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    std::vector<T::FileId> sFileIdList;
    Converter::convert(fileIdList,sFileIdList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByFileIdList(sessionId,sFileIdList,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByProducerId(sessionId,producerId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByProducerName(sessionId,producerName,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getFileInfoListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByGenerationId(sessionId,generationId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListByGenerationName(sessionId,generationName,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::Int32 maxRecords, SmartMet::C::FileInfoList_out fileInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::FileInfoList sFileInfoList;
    C::FileInfoList *corbaFileInfoList = new C::FileInfoList();
    fileInfoList = corbaFileInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getFileInfoListBySourceId(sessionId,sourceId,startFileId,maxRecords,sFileInfoList);

    if (result == 0)
      Converter::convert(sFileInfoList,*corbaFileInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoCountByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoCountByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getFileInfoCountBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::addEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::EventInfo& eventInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::EventInfo sEventInfo;
    Converter::convert(eventInfo,sEventInfo);

    int result = mService->addEventInfo(sessionId,sEventInfo);
    if (result == 0)
      Converter::convert(sEventInfo,eventInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getLastEventInfo(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventInfo_out eventInfo)
{
  FUNCTION_TRACE
  try
  {
    T::EventInfo sEventInfo;
    C::EventInfo *corbaEventInfo = new C::EventInfo();
    eventInfo = corbaEventInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLastEventInfo(sessionId,requestingServerId,sEventInfo);

    if (result == 0)
      Converter::convert(sEventInfo,*corbaEventInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getEventInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32 requestingServerId, SmartMet::C::EventId startEventId, SmartMet::C::Int32 maxRecords, SmartMet::C::EventInfoList_out eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::EventInfoList sEventInfoList;
    C::EventInfoList *corbaEventInfoList = new C::EventInfoList();
    eventInfoList = corbaEventInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getEventInfoList(sessionId,requestingServerId,startEventId,maxRecords,sEventInfoList);

    if (result == 0)
      Converter::convert(sEventInfoList,*corbaEventInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getEventInfoCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::addContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ContentInfo sContentInfo;
    Converter::convert(contentInfo,sContentInfo);

    int result = mService->addContentInfo(sessionId,sContentInfo);
    if (result == 0)
      Converter::convert(sContentInfo,contentInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::setContentInfo(SmartMet::C::SessionId sessionId, const SmartMet::C::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ContentInfo sContentInfo;
    Converter::convert(contentInfo,sContentInfo);

    return mService->setContentInfo(sessionId,sContentInfo);
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::addContentList(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    T::ContentInfoList sContentInfoList;
    Converter::convert(contentInfoList,sContentInfoList);

    int result = mService->addContentList(sessionId,sContentInfoList);
    if (result == 0)
      Converter::convert(sContentInfoList,contentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::deleteContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex)
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





SmartMet::C::Result ServerInterface::deleteContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId)
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





SmartMet::C::Result ServerInterface::deleteContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename)
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




SmartMet::C::Result ServerInterface::deleteContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId)
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





SmartMet::C::Result ServerInterface::deleteContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName)
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





SmartMet::C::Result ServerInterface::deleteContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId)
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





SmartMet::C::Result ServerInterface::deleteContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName)
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





SmartMet::C::Result ServerInterface::deleteContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId)
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





SmartMet::C::Result ServerInterface::getContentInfo(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::MessageIndex messageIndex, SmartMet::C::ContentInfo_out contentInfo)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfo sContentInfo;
    C::ContentInfo *corbaContentInfo = new C::ContentInfo();
    contentInfo = corbaContentInfo;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentInfo(sessionId,fileId,messageIndex,sContentInfo);

    if (result == 0)
      Converter::convert(sContentInfo,*corbaContentInfo);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentList(SmartMet::C::SessionId sessionId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentList(sessionId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByFileId(SmartMet::C::SessionId sessionId, SmartMet::C::FileId fileId, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileId(sessionId,fileId,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByFileIdList(SmartMet::C::SessionId sessionId, const SmartMet::C::FileIdList& fileIdList, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    std::vector<T::FileId> sFileIdList;
    Converter::convert(fileIdList,sFileIdList);

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileIdList(sessionId,sFileIdList,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByFileName(SmartMet::C::SessionId sessionId, const char* filename, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByFileName(sessionId,filename,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByProducerId(sessionId,producerId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByProducerName(sessionId,producerName,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationId(sessionId,generationId,startFileId,startMessageIndex,maxRecords,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationName(sessionId,generationName,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByGenerationIdAndTimeRange(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationIdAndTimeRange(sessionId,generationId,startTime,endTime,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByGenerationNameAndTimeRange(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByGenerationNameAndTimeRange(sessionId,generationName,startTime,endTime,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListBySourceId(SmartMet::C::SessionId sessionId, SmartMet::C::SourceId sourceId, SmartMet::C::FileId startFileId, SmartMet::C::MessageIndex startMessageIndex, SmartMet::C::Int32 maxRecords, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListBySourceId(sessionId,sourceId,startFileId,startMessageIndex,maxRecords,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameter(SmartMet::C::SessionId sessionId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameter(sessionId,parameterKeyType,parameterKey,levelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameterAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndGenerationId(sessionId,generationId,parameterKeyType,parameterKey,levelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameterAndGenerationName(SmartMet::C::SessionId sessionId, const char* generationName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndGenerationName(sessionId,generationName,parameterKeyType,parameterKey,levelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameterAndProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndProducerId(sessionId,producerId,parameterKeyType,parameterKey,levelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameterAndProducerName(SmartMet::C::SessionId sessionId, const char* producerName, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level minLevel, SmartMet::C::Level maxLevel, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp startTime, SmartMet::C::Timestamp endTime, SmartMet::C::UInt32 requestFlags, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterAndProducerName(sessionId,producerName,parameterKeyType,parameterKey,levelId,minLevel,maxLevel,forecastType,forecastNumber,geometryId,startTime,endTime,requestFlags,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListByParameterGenerationIdAndForecastTime(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::Level level, SmartMet::C::ForecastType forecastType, SmartMet::C::ForecastType forecastNumber, SmartMet::C::GeometryId geometryId, SmartMet::C::Timestamp forecastTime, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListByParameterGenerationIdAndForecastTime(sessionId,generationId,parameterKeyType,parameterKey,levelId,level,forecastType,forecastNumber,geometryId,forecastTime,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentListOfInvalidIntegrity(SmartMet::C::SessionId sessionId, SmartMet::C::ContentInfoList_out contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentInfoList;
    C::ContentInfoList *corbaContentInfoList = new C::ContentInfoList();
    contentInfoList = corbaContentInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentListOfInvalidIntegrity(sessionId,sContentInfoList);

    if (result == 0)
      Converter::convert(sContentInfoList,*corbaContentInfoList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentGeometryIdListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryIdList_out geometryIdList)
{
  FUNCTION_TRACE
  try
  {
    std::set<T::GeometryId> sGeometryIdList;
    C::GeometryIdList *corbaGeometryIdList = new C::GeometryIdList();
    geometryIdList = corbaGeometryIdList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentGeometryIdListByGenerationId(sessionId,generationId,sGeometryIdList);

    if (result == 0)
      Converter::convert(sGeometryIdList,*corbaGeometryIdList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}






SmartMet::C::Result ServerInterface::getContentParamListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ContentInfoList_out contentParamList)
{
  FUNCTION_TRACE
  try
  {
    T::ContentInfoList sContentParamList;
    C::ContentInfoList *corbaContentParamList = new C::ContentInfoList();
    contentParamList = corbaContentParamList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamListByGenerationId(sessionId,generationId,sContentParamList);

    if (result == 0)
      Converter::convert(sContentParamList,*corbaContentParamList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentParamKeyListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sParamKeyList;
    C::StringList *corbaParamKeyList = new C::StringList();
    paramKeyList = corbaParamKeyList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamKeyListByGenerationId(sessionId,generationId,parameterKeyType,sParamKeyList);

    if (result == 0)
      Converter::convert(sParamKeyList,*corbaParamKeyList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentParamKeyListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sParamKeyList;
    C::StringList *corbaParamKeyList = new C::StringList();
    paramKeyList = corbaParamKeyList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamKeyListByGenerationAndGeometryId(sessionId,generationId,geometryId,parameterKeyType,sParamKeyList);

    if (result == 0)
      Converter::convert(sParamKeyList,*corbaParamKeyList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentParamKeyListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::ParameterKeyType parameterKeyType, SmartMet::C::StringList_out paramKeyList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sParamKeyList;
    C::StringList *corbaParamKeyList = new C::StringList();
    paramKeyList = corbaParamKeyList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentParamKeyListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,parameterKeyType,sParamKeyList);

    if (result == 0)
      Converter::convert(sParamKeyList,*corbaParamKeyList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentTimeListByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::TimeStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    C::TimeStringList *corbaContentTimeList = new C::TimeStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByGenerationId(sessionId,generationId,sContentTimeList);

    if (result == 0)
      Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentTimeRangeByProducerAndGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime)
{
  FUNCTION_TRACE
  try
  {
    time_t sStartTime = 0;
    time_t sEndTime = 0;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeRangeByProducerAndGenerationId(sessionId,producerId,generationId,sStartTime,sEndTime);

    if (result == 0)
    {
      startTime = sStartTime;
      endTime = sEndTime;
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





SmartMet::C::Result ServerInterface::getContentTimeRangeByGenerationId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::Timestamp& startTime, SmartMet::C::Timestamp& endTime)
{
  FUNCTION_TRACE
  try
  {
    time_t sStartTime = 0;
    time_t sEndTime = 0;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeRangeByGenerationId(sessionId,generationId,sStartTime,sEndTime);

    if (result == 0)
    {
      startTime = sStartTime;
      endTime = sEndTime;
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




SmartMet::C::Result ServerInterface::getContentTimeListByGenerationAndGeometryId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::TimeStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    C::TimeStringList *corbaContentTimeList = new C::TimeStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByGenerationAndGeometryId(sessionId,generationId,geometryId,sContentTimeList);

    if (result == 0)
      Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}




SmartMet::C::Result ServerInterface::getContentTimeListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::TimeStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    C::TimeStringList *corbaContentTimeList = new C::TimeStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,sContentTimeList);

    if (result == 0)
      Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentLevelListByGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    std::set<T::ParamLevel> sContentLevelList;
    C::LevelList *corbaContentLevelList = new C::LevelList();
    contentLevelList = corbaContentLevelList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentLevelListByGenerationGeometryAndLevelId(sessionId,generationId,geometryId,levelId,sContentLevelList);

    if (result == 0)
      Converter::convert(sContentLevelList,*corbaContentLevelList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentLevelListByParameterGenerationGeometryAndLevelId(SmartMet::C::SessionId sessionId, SmartMet::C::GenerationId generationId, SmartMet::C::GeometryId geometryId, const char* parameterKey, SmartMet::C::LevelId levelId, SmartMet::C::LevelList_out contentLevelList)
{
  FUNCTION_TRACE
  try
  {
    std::set<T::ParamLevel> sContentLevelList;
    C::LevelList *corbaContentLevelList = new C::LevelList();
    contentLevelList = corbaContentLevelList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentLevelListByParameterGenerationGeometryAndLevelId(sessionId,generationId,geometryId,parameterKey,levelId,sContentLevelList);

    if (result == 0)
      Converter::convert(sContentLevelList,*corbaContentLevelList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentTimeListByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::TimeStringList_out contentTimeList)
{
  FUNCTION_TRACE
  try
  {
    std::set<std::string> sContentTimeList;
    C::TimeStringList *corbaContentTimeList = new C::TimeStringList();
    contentTimeList = corbaContentTimeList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getContentTimeListByProducerId(sessionId,producerId,sContentTimeList);

    if (result == 0)
      Converter::convert(sContentTimeList,*corbaContentTimeList);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getContentCount(SmartMet::C::SessionId sessionId, SmartMet::C::UInt32& count)
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
      count = static_cast<UInt32>(sCount);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getHashByProducerId(SmartMet::C::SessionId sessionId, SmartMet::C::ProducerId producerId, SmartMet::C::Hash& hash)
{
  FUNCTION_TRACE
  try
  {
    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    hash = 0;
    UInt64 sHash = 0;

    int result = mService->getHashByProducerId(sessionId,producerId,sHash);

    if (result == 0)
      hash = static_cast<UInt64>(sHash);

    return result;
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Service call failed!",nullptr);
    exception.printError();
    return Result::UNEXPECTED_EXCEPTION;
  }
}





SmartMet::C::Result ServerInterface::getLevelInfoList(SmartMet::C::SessionId sessionId, SmartMet::C::LevelInfoList_out levelInfoList)
{
  FUNCTION_TRACE
  try
  {
    T::LevelInfoList sLevelInfoList;
    C::LevelInfoList *corbaLevelInfoList = new C::LevelInfoList();
    levelInfoList = corbaLevelInfoList;

    if (mService == nullptr)
      throw Fmi::Exception(BCP,"Service not initialized!");

    int result = mService->getLevelInfoList(sessionId,sLevelInfoList);

    if (result == 0)
      Converter::convert(sLevelInfoList,*corbaLevelInfoList);

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
