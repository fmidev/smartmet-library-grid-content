#pragma once

#include "ParameterLevelInfo.h"
#include <vector>


namespace SmartMet
{
namespace T
{

typedef std::vector<ParameterLevelInfo*> ParameterLevelInfo_pvec;  //!< Convenience vector of ParameterLevelInfo pointers.

// ====================================================================================
/*! \brief Simple container of ParameterLevelInfo pointers for parameter-level enumeration.
 *
 *  ParameterLevelInfoList is a thin wrapper around a std::vector used to return the
 *  set of parameter-level combinations registered in the Content Server for a given
 *  query context. */
// ====================================================================================

class ParameterLevelInfoList
{
  public:
                            ParameterLevelInfoList();
                            ParameterLevelInfoList(ParameterLevelInfoList& parameterLevelInfoList);
    virtual                 ~ParameterLevelInfoList();

    ParameterLevelInfoList& operator=(ParameterLevelInfoList& parameterLevelInfoList);

    void                    addParameterLevelInfo(ParameterLevelInfo *parameterLevelInfo);
    void                    clear();
    ParameterLevelInfo*     getParameterLevelInfoByIndex(uint index);
    ParameterLevelInfo*     getParameterLevelInfoByIndexNoCheck(uint index);

    ParameterLevelInfo*     getParameterLevelInfo(
                                 T::ParamKeyType parameterKeyType,
                                 T::ParamId parameterKey,
                                 T::ParamLevelId parameterLevelId,
                                 T::ParamLevel level);

    uint                    getLength();
    void                    print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    ParameterLevelInfo_pvec mList;  //!< Underlying vector of ParameterLevelInfo pointers.
};



}
}
