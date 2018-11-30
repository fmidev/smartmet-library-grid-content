#include "ValueRecordList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


ValueRecordList::ValueRecordList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ValueRecordList::ValueRecordList(ValueRecordList& valueRecordList)
{
  try
  {
    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != nullptr)
        mList.push_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ValueRecordList::ValueRecordList(ContentInfoList& contentInfoList,CoordinateType coordinateType,short interpolationMethod,double x,double y)
{
  try
  {
    uint sz = contentInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      ValueRecord *rec = new ValueRecord();
      rec->mFileId = contentInfo->mFileId;
      rec->mMessageIndex = contentInfo->mMessageIndex;
      rec->mCoordinateType = coordinateType;
      rec->mAreaInterpolationMethod = interpolationMethod;
      rec->mX = x;
      rec->mY = y;
      mList.push_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ValueRecordList::~ValueRecordList()
{
  try
  {
    uint sz = mList.size();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = mList[t];
      if (rec != nullptr)
        delete rec;
    }
    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ValueRecordList& ValueRecordList::operator=(ValueRecordList& valueRecordList)
{
  try
  {
    if (&valueRecordList == this)
      return *this;

    clear();
    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != nullptr)
        mList.push_back(new ValueRecord(*rec));
    }
    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




void ValueRecordList::addValueRecord(ValueRecord *valueRecord)
{
  try
  {
    mList.push_back(valueRecord);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ValueRecordList::clear()
{
  try
  {
    uint sz = mList.size();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = mList[t];
      if (rec != nullptr)
        delete rec;
    }
    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





ValueRecord* ValueRecordList::getValueRecordByIndex(uint index) const
{
  try
  {
    if (index >= mList.size())
      return nullptr;

    return mList[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




uint ValueRecordList::getLength() const
{
  try
  {
    return mList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ValueRecordList::print(std::ostream& stream,uint level,uint optionFlags) const
{
  try
  {
    stream << space(level) << "ValueRecordList\n";
    stream << space(level) << "- mList = \n";

    uint sz = mList.size();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = mList[t];
      if (rec != nullptr)
        rec->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}


}
}
