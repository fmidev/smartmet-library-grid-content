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
    firstItem = NULL;
    lastItem = NULL;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





EventInfoList::EventInfoList(EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    eventInfoList.lock();
    mMaxLength = eventInfoList.mMaxLength;
    firstItem = NULL;
    lastItem = NULL;
    mLength = 0;

    EventInfo *it = eventInfoList.getFirstEvent();
    while (it != NULL)
    {
      addEventInfo(it->duplicate());
      it = it->nextItem;
    }

    eventInfoList.unlock();
  }
  catch (...)
  {
    eventInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




EventInfoList::~EventInfoList()
{
  FUNCTION_TRACE
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void EventInfoList::operator=(EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    if (&eventInfoList == this)
      return;

    clear();

    AutoThreadLock lock(&mThreadLock);
    eventInfoList.lock();

    EventInfo *it = eventInfoList.getFirstEvent();
    while (it != NULL)
    {
      addEventInfo(it->duplicate());
      it = it->nextItem;
    }

    eventInfoList.unlock();
  }
  catch (...)
  {
    eventInfoList.unlock();
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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

    if (lastItem != NULL)
    {
      lastItem->nextItem = dataEventInfo;
      dataEventInfo->prevItem = lastItem;
      dataEventInfo->nextItem = NULL;
      lastItem = dataEventInfo;
    }
    else
    {
      firstItem = dataEventInfo;
      lastItem = dataEventInfo;
      dataEventInfo->prevItem = NULL;
      dataEventInfo->nextItem = NULL;
    }
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfoList::addEventInfoToBegin(EventInfo *dataEventInfo)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (firstItem != NULL)
    {
      firstItem->prevItem = dataEventInfo;
      dataEventInfo->nextItem = firstItem;
      dataEventInfo->prevItem = NULL;
      firstItem = dataEventInfo;
    }
    else
    {
      firstItem = dataEventInfo;
      lastItem = dataEventInfo;
      dataEventInfo->prevItem = NULL;
      dataEventInfo->nextItem = NULL;
    }
    mLength++;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfoList::deleteFirstEvent()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (firstItem != NULL)
    {
      EventInfo *it = firstItem;
      firstItem = firstItem->nextItem;
      delete it;
      if (firstItem != NULL)
        firstItem->prevItem = NULL;
      else
        lastItem = NULL;

      mLength--;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfoList::deleteLastEvent()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    if (lastItem != NULL)
    {
      EventInfo *it = lastItem;
      lastItem = lastItem->prevItem;
      delete it;
      if (lastItem != NULL)
        lastItem->nextItem = NULL;
      else
        firstItem = NULL;

      mLength--;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




void EventInfoList::clear()
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);
    EventInfo *it = firstItem;
    while (it != NULL)
    {
      EventInfo *eventInfo = it;
      it = it->nextItem;
      delete eventInfo;
    }

    firstItem = NULL;
    lastItem = NULL;
    mLength = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





EventInfo* EventInfoList::getEventInfo(unsigned long long eventId)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    EventInfo *it = lastItem;
    while (it != NULL)
    {
      if (it->mEventId == eventId)
        return it;

      it = it->prevItem;
    }

    return NULL;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfoList::getEventInfoList(T::EventId startEventId,uint maxRecords,T::EventInfoList& eventInfoList)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    if (firstItem == NULL)
      return;

    EventInfo *it = lastItem;
    while (it != NULL  &&  it->mEventId > startEventId)
    {
      it = it->prevItem;
    }

    if (it == NULL)
      it = firstItem;

    while (it != NULL)
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void EventInfoList::writeToFile(std::string filename)
{
  FUNCTION_TRACE
  try
  {
    AutoThreadLock lock(&mThreadLock);

    FILE *file = fopen(filename.c_str(),"w");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot create the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    EventInfo *it = firstItem;
    while (it != NULL)
    {
      fprintf(file,"%s\n",it->getCsv().c_str());
      it = it->nextItem;
    }

    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
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
    while (it != NULL)
    {
      it->print(stream,level+1,optionFlags);
      it = it->nextItem;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
