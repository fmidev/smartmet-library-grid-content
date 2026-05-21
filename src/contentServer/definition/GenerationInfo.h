#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Metadata record describing one model generation (forecast run) registered
 *         in the Content Server.
 *
 *  A generation groups all grid files produced by a single execution of a numerical
 *  weather model. It belongs to a producer and carries an analysis time, a status flag
 *  indicating whether the run is complete, and time bounds derived from its content. */
// ====================================================================================

class GenerationInfo
{
  public:
                     GenerationInfo();
                     GenerationInfo(const GenerationInfo& generationInfo);
                     GenerationInfo(const char *csv);
    virtual          ~GenerationInfo();

    std::string      getCsv();
    std::string      getCsvHeader();
    void             setCsv(const char *csv);
    void             setCsv(const std::string& csv);

    int              compare(uint comparisonMethod,GenerationInfo *generationInfo);
    GenerationInfo&  operator=(const GenerationInfo& generationInfo);
    GenerationInfo*  duplicate();
    void             print(std::ostream& stream,uint level,uint optionFlags);

    T::GenerationId  mGenerationId;       //!< Unique numeric identifier of the generation.
    uint             mGenerationType;     //!< Classification code for the generation type.
    T::ProducerId    mProducerId;         //!< Identifier of the owning producer.
    std::string      mName;               //!< Short name of the generation (often the analysis time string).
    std::string      mDescription;        //!< Human-readable description of the generation.
    std::string      mAnalysisTime;       //!< ISO-8601 analysis (reference) time of the model run.
    uchar            mStatus;             //!< Lifecycle status (see GenerationInfo::Status constants).
    uint             mFlags;              //!< Bitmask flags (see GenerationInfo::Flags constants).
    T::StorageId     mStorageId;          //!< Storage partition identifier.
    T::SourceId      mSourceId;           //!< Source system from which the generation was ingested.
    time_t           mDeletionTime;       //!< Timestamp after which the generation may be removed (0 = never).
    time_t           mModificationTime;   //!< Timestamp of the last record modification.
    time_t           mContentStartTime;   //!< Earliest forecast valid time among this generation's content records.
    time_t           mContentEndTime;     //!< Latest forecast valid time among this generation's content records.
    std::size_t      mContentHash;        //!< Hash of the content for change detection.

    /*! \brief Lifecycle status constants for a generation. */
    class Status
    {
      public:
        const static uchar Disabled    = 0;  //!< Generation should not be accessed.
        const static uchar Ready       = 1;  //!< Generation process is complete and all data is available.
        const static uchar Running     = 2;  //!< Generation is accessible but the model run is still active.
    };

    /*! \brief Bitmask flags for generation lifecycle management. */
    class Flags
    {
      public:
        static const uint DeletedGeneration  = 4; //!< Generation is logically deleted.
    };

    /*! \brief Sorting-key constants used by GenerationInfoList. */
    class ComparisonMethod
    {
      public:
        static const uint none                         = 0;  //!< No comparison / unsorted.
        static const T::GenerationId generationId      = 1;  //!< Sort by generation id.
        static const uint generationName               = 2;  //!< Sort by generation name.
        static const uint analysisTime_generationId    = 3;  //!< Sort by analysis time, then generation id.
        static const T::ProducerId producerId          = 4;  //!< Sort by producer id.
        static const uint analysisTime                 = 5;  //!< Sort by analysis time.
    };

};


typedef GenerationInfo* GenerationInfoPtr;  //!< Non-owning pointer to a GenerationInfo record.


}
}
