#include "LevelInfoList.h"
#include <macgyver/Exception.h>
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      if (info != nullptr)
        mList.emplace_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





LevelInfoList::~LevelInfoList()
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





LevelInfoList& LevelInfoList::operator=(LevelInfoList& levelInfoList)
{
  try
  {
    if (&levelInfoList == this)
      return *this;

    clear();

    uint sz = levelInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      LevelInfo *info = levelInfoList.getLevelInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.emplace_back(info->duplicate());
    }
    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void LevelInfoList::addLevelInfo(LevelInfo* levelInfo)
{
  try
  {
    mList.emplace_back(levelInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





LevelInfo* LevelInfoList::getLevelInfoByIndex(uint index)
{
  try
  {
    if (index >= mList.size())
      return nullptr;

    return mList.at(index);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint LevelInfoList::getLength()
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
