#pragma once

#include <grid-files/grid/Typedefs.h>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Value object describing a single vertical level available for a parameter.
 *
 *  LevelInfo records the FMI parameter name, its associated level-type identifier, and
 *  the concrete numeric level value. Lists of LevelInfo are used to enumerate the
 *  vertical levels present in the Content Server for a given producer and parameter. */
// ====================================================================================

class LevelInfo
{
  public:
                    LevelInfo();
                    LevelInfo(const LevelInfo& levelInfo);

                    LevelInfo(
                        T::ProducerId producerId,
                        std::string fmiParameterName,
                        T::ParamLevelId fmiParameterLevelId,
                        T::ParamLevel parameterLevel);

    virtual         ~LevelInfo();

    LevelInfo&      operator=(const LevelInfo& levelInfo);

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(const std::string& csv);
    LevelInfo*      duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    T::ProducerId   mProducerId;           //!< Identifier of the producer that provides this level.
    std::string     mFmiParameterName;    //!< FMI parameter name string (e.g. "T-K").
    T::ParamLevelId mFmiParameterLevelId; //!< FMI level type identifier.
    T::ParamLevel   mParameterLevel;      //!< Numeric vertical level value (e.g. 850 for 850 hPa).
};



}
}
