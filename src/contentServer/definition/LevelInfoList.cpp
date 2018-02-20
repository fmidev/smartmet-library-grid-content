#include "LevelInfoList.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


LevelInfoList::LevelInfoList()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





LevelInfoList::LevelInfoList(LevelInfoList& levelInfoList)
{
  try
  {
    uint sz = levelInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      LevelInfo *info = levelInfoList.getLevelInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





LevelInfoList::~LevelInfoList()
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





void LevelInfoList::operator=(LevelInfoList& levelInfoList)
{
  try
  {
    if (&levelInfoList == this)
      return;

    clear();

    uint sz = levelInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      LevelInfo *info = levelInfoList.getLevelInfoByIndexNoCheck(t);
      if (info != NULL)
        mList.push_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void LevelInfoList::addLevelInfo(LevelInfo* levelInfo)
{
  try
  {
    mList.push_back(levelInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void LevelInfoList::clear()
{
  try
  {
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      delete(mList[t]);

    mList.clear();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





LevelInfo* LevelInfoList::getLevelInfoByIndex(uint index)
{
  try
  {
    if (index >= mList.size())
      return NULL;

    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




LevelInfo* LevelInfoList::getLevelInfoByIndexNoCheck(uint index)
{
  try
  {
    return mList.at(index);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




uint LevelInfoList::getLength()
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





void LevelInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "LevelInfoList\n";
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
      mList[t]->print(stream,level+1,optionFlags);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}




}
}
