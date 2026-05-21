#include "LevelInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for LevelInfoList. */

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





/*! \brief Copy constructor for LevelInfoList. */

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





/*! \brief Destructor for LevelInfoList. */

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





/*! \brief Copy assignment operator for LevelInfoList. */

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





/*! \brief Add a LevelInfo record to the list. */

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





/*! \brief Remove all entries from the list. */

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





/*! \brief Return the LevelInfo at the given list index. */

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




/*! \brief Return the LevelInfo at the given list index without bounds checking. */

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




/*! \brief Return the number of entries in the list. */

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





/*! \brief Print the list contents to the given stream. */

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
