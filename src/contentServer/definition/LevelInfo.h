#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace T
{


class LevelInfo
{
  public:
                    LevelInfo();
                    LevelInfo(const LevelInfo& levelInfo);

                    LevelInfo(
                        uint producerId,
                        std::string fmiParameterName,
                        T::ParamLevelId fmiParameterLevelId,
                        T::ParamLevelId grib1ParameterLevelId,
                        T::ParamLevelId grib2ParameterLevelId,
                        T::ParamLevel parameterLevel);

    virtual         ~LevelInfo();

    LevelInfo&      operator=(const LevelInfo& levelInfo);

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(std::string csv);
    LevelInfo*      duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    uint            mProducerId;
    std::string     mFmiParameterName;
    T::ParamLevelId mFmiParameterLevelId;
    T::ParamLevelId mGrib1ParameterLevelId;
    T::ParamLevelId mGrib2ParameterLevelId;
    T::ParamLevel   mParameterLevel;
};



}
}
