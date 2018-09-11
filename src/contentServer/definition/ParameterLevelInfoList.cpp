#include "ParameterLevelInfoList.h"
#include <grid-files/common/Exception.h>
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
        mList.push_back(info->duplicate());
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
        mList.push_back(info->duplicate());
    }
    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void ParameterLevelInfoList::addParameterLevelInfo(ParameterLevelInfo* parameterLevelInfo)
{
  try
  {
    mList.push_back(parameterLevelInfo);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




ParameterLevelInfo* ParameterLevelInfoList::getParameterLevelInfo(
                        T::ParamKeyType parameterKeyType,
                        T::ParamId parameterKey,
                        T::ParamLevelIdType parameterLevelIdType,
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
          mList[t]->mParameterLevelIdType == parameterLevelIdType &&
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
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
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
