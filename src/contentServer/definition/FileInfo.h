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

    enum class ComparisonMethod
    {
      none = 0,     // No comparison
      fileId = 1,   // Comparison according to the file id
      fileName = 2  // Comparison according to the file name
    };

  public:
                        FileInfo();
                        FileInfo(FileInfo& fileInfo);
                        FileInfo(const FileInfo& fileInfo);
                        FileInfo(uint producerId,uint generationId,uint groupFlags,T::FileType type,std::string filename,uint sourceId);
                        FileInfo(const char *csv);
     virtual            ~FileInfo();

     std::string        getCsv();
     std::string        getCsvHeader();
     void               setCsv(const char *csv);
     void               setCsv(std::string csv);

     void               operator=(FileInfo& fileInfo);
     void               operator=(const FileInfo& fileInfo);
     int                compare(ComparisonMethod comparisonMethod,FileInfo *fileInfo);
     FileInfo*          duplicate();
     void               print(std::ostream& stream,uint level,uint optionFlags);

     /*! \brief A file can belong to 32 different groups. Each bit of this attribute represents a group.
      * The basic idea is that we can divide files into different groups according to some criteria.
      * After that we can use this information for different purposes. For example, we can define that
      * if a file belongs to group N then the data server X will deliver data from the current file.
      * On the other hand, if the file belongs to group M then the data server Y takes care of the
      * data delivery from the current file. *** At the moment, group information is not used for
      * any purposes. *****/
     uint               mGroupFlags;

     /*! \brief The producer identifier. */
     uint               mProducerId;

     /*! \brief The generation identifier. */
     uint               mGenerationId;

     /*! \brief The file identifier. */
     uint               mFileId;

     /*! \brief The file type. */
     T::FileType        mFileType;

     /*! \brief The file name. */
     std::string        mName;

     /*! \brief The file flags are used for indicating same additional information related
      * to the current file. For example, if the bit 0 is set to '1' then the content
      * of the current file is predefined, which means that the data servers do not need
      * to open the file and register its content to the content server. */
     uint               mFlags;

     uint               mSourceId;
};


typedef FileInfo* FileInfoPtr;



enum class FileInfoFlags
{
  CONTENT_PREDEFINED   = 1,
  CONTENT_VIRTUAL      = 2,
  FILE_DELETED         = 4
};


}
}
