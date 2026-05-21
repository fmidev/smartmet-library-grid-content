#include "ParameterLevelInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


/*! \brief Default constructor for ParameterLevelInfoList. */

ParameterLevelInfoList::ParameterLevelInfoList()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Copy constructor for ParameterLevelInfoList. */

ParameterLevelInfoList::ParameterLevelInfoList(ParameterLevelInfoList& parameterLevelInfoList)
{
  try
  {
    uint sz = parameterLevelInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ParameterLevelInfo *info = parameterLevelInfoList.getParameterLevelInfoByIndexNoCheck(t);
      if (info != nullptr)
        mList.emplace_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Destructor for ParameterLevelInfoList. */

ParameterLevelInfoList::~ParameterLevelInfoList()
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





/*! \brief Copy assignment operator for ParameterLevelInfoList. */

ParameterLevelInfoList& ParameterLevelInfoList::operator=(ParameterLevelInfoList& parameterLevelInfoList)
{
  try
  {
    if (&parameterLevelInfoList == this)
      return *this;

    clear();

    uint sz = parameterLevelInfoList.getLength();
    for (uint t=0; t<sz; t++)
    {
      ParameterLevelInfo *info = parameterLevelInfoList.getParameterLevelInfoByIndexNoCheck(t);
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





/*! \brief Add a ParameterLevelInfo record to the list. */

void ParameterLevelInfoList::addParameterLevelInfo(ParameterLevelInfo* parameterLevelInfo)
{
  try
  {
    mList.emplace_back(parameterLevelInfo);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Remove all entries from the list. */

void ParameterLevelInfoList::clear()
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





/*! \brief Return the ParameterLevelInfo at the given list index. */

ParameterLevelInfo* ParameterLevelInfoList::getParameterLevelInfoByIndex(uint index)
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




/*! \brief Return the ParameterLevelInfo at the given list index without bounds checking. */

ParameterLevelInfo* ParameterLevelInfoList::getParameterLevelInfoByIndexNoCheck(uint index)
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




/*! \brief Return the ParameterLevelInfo matching the given parameter key and level fields. */

ParameterLevelInfo* ParameterLevelInfoList::getParameterLevelInfo(
                        T::ParamKeyType parameterKeyType,
                        T::ParamId parameterKey,
                        T::ParamLevelId parameterLevelId,
                        T::ParamLevel level)

{
  try
  {
    std::size_t sz = mList.size();
    for (std::size_t t=0; t<sz; t++)
    {
      if (mList[t]->mParameterKeyType == parameterKeyType &&
          mList[t]->mParameterKey == parameterKey &&
          mList[t]->mParameterLevelId == parameterLevelId &&
          mList[t]->mLevel == level)
      {
        return mList[t];
      }
    }
    return nullptr;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





/*! \brief Return the number of entries in the list. */

uint ParameterLevelInfoList::getLength()
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

void ParameterLevelInfoList::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ParameterLevelInfoList\n";
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
