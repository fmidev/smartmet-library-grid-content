#pragma once

#include "ParameterLevelInfo.h"
#include <vector>


namespace SmartMet
{
namespace T
{


class ParameterLevelInfoList
{
  public:
                          ParameterLevelInfoList();
                          ParameterLevelInfoList(ParameterLevelInfoList& parameterLevelInfoList);
     virtual              ~ParameterLevelInfoList();

     void                 operator=(ParameterLevelInfoList& parameterLevelInfoList);

     void                 addParameterLevelInfo(ParameterLevelInfo *parameterLevelInfo);
     void                 clear();
     ParameterLevelInfo*  getParameterLevelInfoByIndex(uint index);
     ParameterLevelInfo*  getParameterLevelInfoByIndexNoCheck(uint index);

     ParameterLevelInfo*  getParameterLevelInfo(
                             T::ParamKeyType parameterKeyType,
                             T::ParamId parameterKey,
                             T::ParamLevelIdType parameterLevelIdType,
                             T::ParamLevelId parameterLevelId,
                             T::ParamLevel level);

     uint                 getLength();
     void                 print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<ParameterLevelInfo*>  mList;
};



}
}
