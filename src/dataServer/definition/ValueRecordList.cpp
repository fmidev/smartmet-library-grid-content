#include "ValueRecordList.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
      if (rec != NULL)
        mList.push_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ValueRecordList::ValueRecordList(ContentInfoList& contentInfoList,CoordinateType coordinateType,InterpolationMethod interpolationMethod,double x,double y)
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
      rec->mInterpolationMethod = interpolationMethod;
      rec->mX = x;
      rec->mY = y;
      mList.push_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ValueRecordList::~ValueRecordList()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ValueRecordList::operator=(ValueRecordList& valueRecordList)
{
  try
  {
    clear();
    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != NULL)
        mList.push_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
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
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ValueRecordList::clear()
{
  try
  {
    uint sz = (uint)mList.size();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = mList[t];
      if (rec != NULL)
        delete rec;
    }
    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ValueRecord* ValueRecordList::getValueRecordByIndex(uint index)
{
  try
  {
    if (index >= (uint)mList.size())
      return NULL;

    return mList[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




uint ValueRecordList::getLength()
{
  try
  {
    return (uint)mList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ValueRecordList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ValueRecordList\n";
    stream << space(level) << "- mList = \n";

    uint sz = (uint)mList.size();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = mList[t];
      if (rec != NULL)
        rec->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}


}
}
