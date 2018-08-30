#pragma once

#include "LevelInfo.h"
#include <vector>


namespace SmartMet
{
namespace T
{


typedef std::vector<LevelInfo*> LevelInfo_pvec;


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

    LevelInfo_pvec  mList;
};



}
}
