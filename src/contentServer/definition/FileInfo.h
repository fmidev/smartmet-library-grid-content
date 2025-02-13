#pragma once

#include <grid-files/grid/Typedefs.h>
#include <string>



namespace SmartMet
{
namespace T
{


class FileInfo
{
  public:

                  FileInfo();
                  FileInfo(const FileInfo& fileInfo);

                  FileInfo(
                      uint producerId,
                      uint generationId,
                      uchar type,
                      const std::string& filename,
                      uint sourceId);

                  FileInfo(const char *csv);
    virtual       ~FileInfo();

    FileInfo&     operator=(const FileInfo& fileInfo);

    std::string   getCsv();
    std::string   getCsvHeader();
    void          setCsv(const char *csv);
    void          setCsv(const std::string& csv);

    int           compare(uint comparisonMethod,FileInfo *fileInfo);
    FileInfo*     duplicate();
    void          print(std::ostream& stream,uint level,uint optionFlags);

    /*! \brief The producer identifier. */
    uint          mProducerId;

    /*! \brief The generation identifier. */
    uint          mGenerationId;

    /*! \brief The file identifier. */
    uint          mFileId;

    /*! \brief The file protocol. (0 = None, 1 = HTTPS, 2 = HTTP) */
    uint          mProtocol;

    /*! \brief The file server type (0 = Unknow, 1= Filesys, 2 = S3, 3 = Httpd. */
    uint          mServerType;

    /*! \brief The file server. */
    std::string   mServer;

    /*! \brief The file type. */
    uchar         mFileType;

    /*! \brief The file name. */
    std::string   mName;


    /*! \brief The file flags are used for indicating same additional information related
     * to the current file. For example, if the bit 0 is set to '1' then the content
     * of the current file is predefined, which means that the data servers do not need
     * to open the file and register its content to the content server. */
    uint          mFlags;

    uint          mSourceId;

    time_t        mModificationTime;

    time_t        mDeletionTime;

    ulonglong     mSize;

    uchar         mStatus;

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

    class ComparisonMethod
    {
      public:
        static const uint none              = 0;    // No comparison
        static const uint fileId            = 1;    // Comparison according to the file id
        static const uint fileName          = 2;    // Comparison according to the file name
    };

    class Flags
    {
      public:
        static const uint UnusedFlag            = 1;
        static const uint DeletedFile           = 4;     // The file is deleted
        static const uint LocalCacheRecommended = 8;     // The file shoud be cached locally if possible
        static const uint LocalCacheInUse       = 16;    // The file is cached locally
    };

    class ServerType
    {
      public:
        static const uint Unknown          = 0;
        static const uint Filesys          = 1;    // Local file system
        static const uint S3               = 2;    // S3
        static const uint THREDDS          = 3;    // THREDDS data server
        static const uint HTTPD            = 4;    // HTTPD
    };

    class Protocol
    {
      public:
        static const uint None             = 0;
        static const uint HTTP             = 1;
        static const uint HTTPS            = 2;
    };

};


typedef FileInfo* FileInfoPtr;





}
}
