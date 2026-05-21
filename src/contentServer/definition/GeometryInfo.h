#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Metadata record linking a grid geometry to a specific generation and level type
 *         in the Content Server.
 *
 *  GeometryInfo records are created when content is registered for a particular
 *  generation-geometry-level combination. They allow callers to quickly enumerate
 *  which geometries are available for a given model run without scanning all content. */
// ====================================================================================

class GeometryInfo
{
  public:

                    GeometryInfo();
                    GeometryInfo(const GeometryInfo& geometryInfo);
                    GeometryInfo(const char *csv);
    virtual         ~GeometryInfo();

    GeometryInfo&   operator=(const GeometryInfo& geometryInfo);

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(const std::string& csv);

    int             compare(uint comparisonMethod,GeometryInfo *geometryInfo);
    GeometryInfo*   duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    T::ProducerId   mProducerId;        //!< Identifier of the owning producer.
    T::GenerationId mGenerationId;      //!< Identifier of the model generation this geometry belongs to.
    T::GeometryId   mGeometryId;        //!< Grid geometry identifier.
    T::ParamLevelId mLevelId;           //!< Vertical level type identifier associated with this geometry record.
    uint            mFlags;             //!< Bitmask flags (see GeometryInfo::Flags constants).
    T::StorageId    mStorageId;         //!< Storage partition identifier.
    T::SourceId     mSourceId;          //!< Source system from which this record was ingested.
    time_t          mModificationTime;  //!< Timestamp of the last record modification.
    time_t          mDeletionTime;      //!< Timestamp after which the record may be removed (0 = never).
    uchar           mStatus;            //!< Lifecycle status (see GeometryInfo::Status constants).

    /*! \brief Sorting-key constants used by GeometryInfoList. */
    class ComparisonMethod
    {
      public:
        static const uint none                    = 0; //!< No comparison / unsorted.
        static const T::GenerationId generationId = 1; //!< Sort by generation id.
        static const T::ProducerId producerId     = 2; //!< Sort by producer id.
    };

    /*! \brief Lifecycle status constants for a geometry record. */
    class Status
    {
      public:
        const static uchar Disabled    = 0;  //!< Geometry should not be accessed.
        const static uchar Ready       = 1;  //!< Data for this geometry is complete and available.
        const static uchar Running     = 2;  //!< Geometry is accessible but the associated generation is still active.
    };

    /*! \brief Bitmask flags for geometry lifecycle management. */
    class Flags
    {
      public:
        static const uint DeletedGeometry   = 4; //!< Geometry record is logically deleted.
    };
};


typedef GeometryInfo* GeometryInfoPtr;  //!< Non-owning pointer to a GeometryInfo record.





}
}
