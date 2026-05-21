#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Lightweight value object identifying a single available forecast valid time.
 *
 *  ForecastTime captures the combination of storage, source, generation, geometry,
 *  forecast type/number, and the time string itself. It is used when enumerating the
 *  distinct forecast times available for a given parameter and geometry. */
// ====================================================================================

class ForecastTime
{
  public:
                       ForecastTime();
                       ForecastTime(const ForecastTime& forecastTime);
                       ForecastTime(const char *csv);
     virtual           ~ForecastTime();

     std::string       getCsv();
     void              setCsv(const char *csv);
     void              setCsv(const std::string& csv);

     void              print(std::ostream& stream,uint level,uint optionFlags);

     T::StorageId      mStorageId;        //!< Storage partition identifier.
     T::SourceId       mSourceId;         //!< Source system identifier.
     T::GenerationId   mGenerationId;     //!< Generation (model run) identifier.
     T::GeometryId     mGeometryId;       //!< Grid geometry identifier.
     T::ForecastType   mForecastType;     //!< Forecast type code.
     T::ForecastNumber mForecastNumber;   //!< Ensemble member or perturbation number.
     std::string       mForecastTime;     //!< Forecast valid time as an ISO-8601 string.
     std::string       mModificationTime; //!< ISO-8601 timestamp of the last record modification.
};



}
}
