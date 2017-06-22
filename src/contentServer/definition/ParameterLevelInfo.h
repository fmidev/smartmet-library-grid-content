#pragma once

#include "grid-files/grid/Typedefs.h"


namespace SmartMet
{
namespace T
{


class ParameterLevelInfo
{
  public:
                          ParameterLevelInfo();
                          ParameterLevelInfo(ParameterLevelInfo& levelInfo);
                          ParameterLevelInfo(T::ParamKeyType parameterKeyType,
                                             T::ParamId parameterKey,
                                             T::ParamLevelIdType parameterLevelIdType,
                                             T::ParamLevelId parameterLevelId,
                                             T::ParamLevel level);
     virtual              ~ParameterLevelInfo();

     void                 operator=(ParameterLevelInfo& levelInfo);
     ParameterLevelInfo*  duplicate();
     void                 print(std::ostream& stream,uint level,uint optionFlags);

     T::ParamKeyType      mParameterKeyType;
     T::ParamId           mParameterKey;
     T::ParamLevelIdType  mParameterLevelIdType;
     T::ParamLevelId      mParameterLevelId;
     T::ParamLevel        mLevel;
};



}
}
