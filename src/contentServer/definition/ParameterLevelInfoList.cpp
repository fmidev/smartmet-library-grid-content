#include "ParameterLevelInfoList.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


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
