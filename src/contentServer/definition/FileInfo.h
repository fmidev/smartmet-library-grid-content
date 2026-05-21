#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Metadata record describing a grid data file registered in the Content Server.
 *
 *  FileInfo stores the location, format, and lifecycle attributes of a single file
 *  (GRIB, NetCDF, or QueryData) that has been registered with the Content Server. It
 *  tracks the server type and protocol so that DataServer implementations can open
 *  files from local filesystems, S3 buckets, or HTTP endpoints transparently. */
// ====================================================================================

class FileInfo
{
  public:

                    FileInfo();
                    FileInfo(const FileInfo& fileInfo);

                    FileInfo(
                      T::ProducerId producerId,
                      T::GenerationId generationId,
                      uchar type,
                      const std::string& filename,
                      T::SourceId sourceId);

                    FileInfo(const char *csv);
    virtual         ~FileInfo();

    FileInfo&       operator=(const FileInfo& fileInfo);

    std::string     getCsv();
    std::string     getCsvHeader();
    void            setCsv(const char *csv);
    void            setCsv(const std::string& csv);

    int             compare(uint comparisonMethod,FileInfo *fileInfo);
    FileInfo*       duplicate();
    void            print(std::ostream& stream,uint level,uint optionFlags);

    /*! \brief The producer identifier. */
    T::ProducerId   mProducerId;

    /*! \brief The generation identifier. */
    T::GenerationId mGenerationId;

    /*! \brief The file identifier. */
    T::FileId       mFileId;

    /*! \brief The file protocol. (0 = None, 1 = HTTPS, 2 = HTTP) */
    uint            mProtocol;

    /*! \brief The file server type (0 = Unknow, 1= Filesys, 2 = S3, 3 = Httpd. */
    uint            mServerType;

    /*! \brief The file server. */
    std::string     mServer;

    /*! \brief The file type. */
    uchar           mFileType;

    /*! \brief The file name. */
    std::string     mName;


    /*! \brief The file flags are used for indicating same additional information related
     * to the current file. For example, if the bit 0 is set to '1' then the content
     * of the current file is predefined, which means that the data servers do not need
     * to open the file and register its content to the content server. */
    uint            mFlags;

    T::StorageId    mStorageId;        //!< Storage partition identifier.

    T::SourceId     mSourceId;         //!< Source system from which the file was ingested.

    time_t          mModificationTime; //!< Timestamp of the last modification of this record.

    time_t          mDeletionTime;     //!< Timestamp after which the file may be removed (0 = never).

    UInt64          mSize;             //!< File size in bytes.

    uchar           mStatus;           //!< Lifecycle status (see FileInfo::ServerType for analogous codes).

    /* The FileInfo records can be sorted in different ways when they are stored
     * into the FileInfoList object. That's why the FileInfo class contains
     * the "compare" -method, which compares objects of the current class
     * in according to the given "comparison method". This method is called
     * when the FileInfoList object is sorted by the "sort" method.
     *
     * If we want to find FileInfo objects fast from a long array, then we
     * need to sort the array. After that we can use binary search algorithm
     * to find the records. On the other hand, if we want to keep the array
     * sorted all the time then we have to add objects so that the array
     * stays in correct order.
     */

    /*! \brief Sorting-key constants controlling FileInfoList sort order. */
    class ComparisonMethod
    {
      public:
        static const uint none              = 0;    //!< No comparison / unsorted.
        static const T::FileId fileId       = 1;    //!< Sort by numeric file id.
        static const uint fileName          = 2;    //!< Sort lexicographically by file name.
    };

    /*! \brief Bitmask flags describing file lifecycle and caching state. */
    class Flags
    {
      public:
        static const uint UnusedFlag            = 1;   //!< Reserved for future use.
        static const uint DeletedFile           = 4;   //!< The file is logically deleted.
        static const uint LocalCacheRecommended = 8;   //!< The file should be cached locally if possible.
        static const uint LocalCacheInUse       = 16;  //!< The file is currently cached locally.
    };

    /*! \brief Identifies the storage technology hosting the file. */
    class ServerType
    {
      public:
        static const uint Unknown          = 0; //!< Unknown or not set.
        static const uint Filesys          = 1; //!< Local file system.
        static const uint S3               = 2; //!< Amazon S3 or compatible object storage.
        static const uint THREDDS          = 3; //!< THREDDS data server.
        static const uint HTTPD            = 4; //!< Generic HTTP server.
    };

    /*! \brief Network protocol used to retrieve the file. */
    class Protocol
    {
      public:
        static const uint None             = 0; //!< No network protocol (local access).
        static const uint HTTP             = 1; //!< Plain HTTP.
        static const uint HTTPS            = 2; //!< HTTPS.
    };

};


typedef FileInfo* FileInfoPtr;  //!< Non-owning pointer to a FileInfo record.





}
}
