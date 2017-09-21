#include "GridValueList.h"
#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{


GridValueList::GridValueList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




GridValueList::GridValueList(GridValueList& gridValueList)
{
  try
  {
    uint sz = gridValueList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GridValue *rec = gridValueList.getGridValueByIndex(t);
      if (rec != NULL)
        mList.push_back(new GridValue(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GridValueList::~GridValueList()
{
  try
  {
    clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridValueList::operator=(GridValueList& gridValueList)
{
  try
  {
    clear();
    uint sz = gridValueList.getLength();
    for (uint t=0; t<sz; t++)
    {
      GridValue *rec = gridValueList.getGridValueByIndex(t);
      if (rec != NULL)
        mList.push_back(new GridValue(*rec));
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridValueList::addGridValue(GridValue *gridValue)
{
  try
  {
    mList.push_back(gridValue);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridValueList::clear()
{
  try
  {
    uint sz = (uint)mList.size();
    for (uint t=0; t<sz; t++)
    {
      GridValue *rec = mList[t];
      if (rec != NULL)
        delete rec;
    }
    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





GridValue* GridValueList::getGridValueByIndex(uint index)
{
  try
  {
    if (index >= (uint)mList.size())
      return NULL;

    return mList[index];
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint GridValueList::getLength()
{
  try
  {
    return (uint)mList.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void GridValueList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "GridValueList\n";
    stream << space(level) << "- mList = \n";

    uint sz = (uint)mList.size();
    for (uint t=0; t<sz; t++)
    {
      GridValue *rec = mList[t];
      if (rec != NULL)
        rec->print(stream,level+2,optionFlags);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}


}
}
