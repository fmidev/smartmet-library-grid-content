#pragma once

#include "LevelInfo.h"
#include <vector>


namespace SmartMet
{
namespace T
{


typedef std::vector<LevelInfo*> LevelInfo_pvec;  //!< Convenience vector of LevelInfo pointers.

// ====================================================================================
/*! \brief Simple container of LevelInfo pointers representing available vertical levels.
 *
 *  LevelInfoList is a thin wrapper around a std::vector used to return enumerations
 *  of the vertical levels registered in the Content Server for a given parameter and
 *  producer combination. */
// ====================================================================================

class LevelInfoList
{
  public:
                    LevelInfoList();
                    LevelInfoList(LevelInfoList& levelInfoList);
    virtual         ~LevelInfoList();

    LevelInfoList&  operator=(LevelInfoList& levelInfoList);

    void            addLevelInfo(LevelInfo *levelInfo);
    void            clear();
    LevelInfo*      getLevelInfoByIndex(uint index);
    LevelInfo*      getLevelInfoByIndexNoCheck(uint index);
    uint            getLength();
    void            print(std::ostream& stream,uint level,uint optionFlags);

  protected:

    LevelInfo_pvec  mList;  //!< Underlying vector of LevelInfo pointers.
};



}
}
