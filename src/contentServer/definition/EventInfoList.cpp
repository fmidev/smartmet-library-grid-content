#include "EventInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/common/AutoThreadLock.h>
#include <grid-files/common/ShowFunction.h>

#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace T
{


EventInfoList::EventInfoList()
{
  FUNCTION_TRACE
  try
  {
    mMaxLength = 3000000;
    firstItem = nullptr;
    lastItem = nullptr;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





EventInfoList::EventInfoList(EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    eventInfoList.lock();
    mMaxLength = eventInfoList.mMaxLength;
    firstItem = nullptr;
    lastItem = nullptr;
    mLength = 0;

    EventInfo *it = eventInfoList.getFirstEvent();
    while (it != nullptr)
    {
      addEventInfo(it->duplicate());
      it = it->nextItem;
    }

    eventInfoList.unlock();
  }
  catch (...)
  {
    eventInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




EventInfoList::~EventInfoList()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    EventInfo *it = firstItem;
    while (it != nullptr)
    {
      EventInfo *eventInfo = it;
      it = it->nextItem;
      delete eventInfo;
    }

    firstItem = nullptr;
    lastItem = nullptr;
    mLength = 0;
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}




EventInfoList& EventInfoList::operator=(EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&eventInfoList == this)
      return *this;

    clear();

    AutoThreadLock lock(&mThreadLock);
    eventInfoList.lock();

    EventInfo *it = eventInfoList.getFirstEvent();
    while (it != nullptr)
    {
      addEventInfo(it->duplicate());
      it = it->nextItem;
    }

    eventInfoList.unlock();
    return *this;
  }
  catch (...)
  {
    eventInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




EventInfo* EventInfoList::getFirstEvent()
{
  FUNCTION_TRACE
  try
  {
    return firstItem;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




EventInfo* EventInfoList::getLastEvent()
{
  FUNCTION_TRACE
  try
  {
    return lastItem;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void EventInfoList::addEventInfo(EventInfo *dataEventInfo)
{
  FUNCTION_TRACE
  try
  {
    while (mLength > mMaxLength)
      deleteFirstEvent();

    AutoThreadLock lock(&mThreadLock);

    if (lastItem != nullptr)
    {
      lastItem->nextItem = dataEventInfo;
      dataEventInfo->prevItem = lastItem;
      dataEventInfo->nextItem = nullptr;
      lastItem = dataEventInfo;
    }
    else
    {
      firstItem = dataEventInfo;
      lastItem = dataEventInfo;
      dataEventInfo->prevItem = nullptr;
      dataEventInfo->nextItem = nullptr;
    }
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::addEventInfoToBegin(EventInfo *dataEventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (firstItem != nullptr)
    {
      firstItem->prevItem = dataEventInfo;
      dataEventInfo->nextItem = firstItem;
      dataEventInfo->prevItem = nullptr;
      firstItem = dataEventInfo;
    }
    else
    {
      firstItem = dataEventInfo;
      lastItem = dataEventInfo;
      dataEventInfo->prevItem = nullptr;
      dataEventInfo->nextItem = nullptr;
    }
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::deleteFirstEvent()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (firstItem != nullptr)
    {
      EventInfo *it = firstItem;
      firstItem = firstItem->nextItem;
      delete it;
      if (firstItem != nullptr)
        firstItem->prevItem = nullptr;
      else
        lastItem = nullptr;

      mLength--;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::deleteLastEvent()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    if (lastItem != nullptr)
    {
      EventInfo *it = lastItem;
      lastItem = lastItem->prevItem;
      delete it;
      if (lastItem != nullptr)
        lastItem->nextItem = nullptr;
      else
        firstItem = nullptr;

      mLength--;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void EventInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    EventInfo *it = firstItem;
    while (it != nullptr)
    {
      EventInfo *eventInfo = it;
      it = it->nextItem;
      delete eventInfo;
    }

    firstItem = nullptr;
    lastItem = nullptr;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint EventInfoList::getLength()
{
  FUNCTION_TRACE
  try
  {
    return mLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





uint EventInfoList::getMaxLength()
{
  FUNCTION_TRACE
  try
  {
    return mMaxLength;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





EventInfo* EventInfoList::getEventInfo(unsigned long long eventId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    EventInfo *it = lastItem;
    while (it != nullptr)
    {
      if (it->mEventId == eventId)
        return it;

      it = it->prevItem;
    }

    return nullptr;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::getEventInfoList(T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    eventInfoList.clear();

    AutoThreadLock lock(&mThreadLock);

    if (firstItem == nullptr)
      return;

    EventInfo *it = lastItem;
    while (it != nullptr  &&  it->mEventId > startEventId)
    {
      it = it->prevItem;
    }

    if (it == nullptr)
      it = firstItem;

    while (it != nullptr)
    {
      if (it->mEventId >= startEventId)
      {
        eventInfoList.addEventInfo(it->duplicate());
        if (eventInfoList.getLength() == maxRecords)
          return;
      }

      it = it->nextItem;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::setMaxLength(uint maxLength)
{
  FUNCTION_TRACE
  try
  {
    mMaxLength = maxLength;
    while (mLength > mMaxLength)
    {
      deleteFirstEvent();
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::lock()
{
  FUNCTION_TRACE
  try
  {
    return mThreadLock.lock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::unlock()
{
  FUNCTION_TRACE
  try
  {
    return mThreadLock.unlock();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void EventInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    FILE *file = fopen(filename.c_str(),"we");
    if (file == nullptr)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    EventInfo *it = firstItem;
    while (it != nullptr)
    {
      fprintf(file,"%s\n",it->getCsv().c_str());
      it = it->nextItem;
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void EventInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    stream << space(level) << "EventInfoList\n";
    EventInfo *it = firstItem;
    while (it != nullptr)
    {
      it->print(stream,level+1,optionFlags);
      it = it->nextItem;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
