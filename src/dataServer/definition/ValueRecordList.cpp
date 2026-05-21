#include "ValueRecordList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor: creates an empty value record list. */

ValueRecordList::ValueRecordList()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Copy constructor: deep-copies all value records from the source list. */

ValueRecordList::ValueRecordList(ValueRecordList& valueRecordList)
{
  try
  {
    uint sz = valueRecordList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ValueRecord *rec = valueRecordList.getValueRecordByIndex(t);
      if (rec != nullptr)
        mList.emplace_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Builds a value record list from content info entries with given coordinate and interpolation parameters. */

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
      mList.emplace_back(new ValueRecord(*rec));
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor: deletes all owned value records. */

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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}




/*! \brief Assignment operator: clears self then deep-copies the source list. */

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
        mList.emplace_back(new ValueRecord(*rec));
    }
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Appends a value record (takes ownership) to the list. */

void ValueRecordList::addValueRecord(ValueRecord *valueRecord)
{
  try
  {
    mList.emplace_back(valueRecord);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Deletes all value records and empties the list. */

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Returns the value record at the given index, or nullptr if out of range. */

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




/*! \brief Returns the number of records in the list. */

uint ValueRecordList::getLength() const
{
  try
  {
    return mList.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Prints the list and all contained value records to the given stream. */

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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
