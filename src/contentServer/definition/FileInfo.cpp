#include "FileInfo.h"
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>


namespace SmartMet
{
namespace T
{


FileInfo::FileInfo()
{
  try
  {
    mFileId = 0;
    mFileType = T::FileTypeValue::Unknown;
    mProducerId = 0;
    mGenerationId = 0;
    mGroupFlags = 0;
    mFlags = 0;
    mSourceId = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo::FileInfo(const FileInfo& fileInfo)
{
  try
  {
    mFileId = fileInfo.mFileId;
    mFileType = fileInfo.mFileType;
    mName = fileInfo.mName;
    mProducerId = fileInfo.mProducerId;
    mGenerationId = fileInfo.mGenerationId;
    mGroupFlags = fileInfo.mGroupFlags;
    mFlags = fileInfo.mFlags;
    mSourceId = fileInfo.mSourceId;
    mModificationTime = fileInfo.mModificationTime;
    mDeletionTime = fileInfo.mDeletionTime;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo::FileInfo(uint producerId,uint generationId,uint groupFlags,uchar type,std::string filename,uint sourceId)
{
  try
  {
    mProducerId = producerId;
    mGenerationId = generationId;
    mGroupFlags = groupFlags;
    mFileId = 0;
    mFileType = type;
    mName = filename;
    mFlags = 0;
    mSourceId = sourceId;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo::FileInfo(const char *csv)
{
  try
  {
    setCsv(csv);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo::~FileInfo()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo& FileInfo::operator=(const FileInfo& fileInfo)
{
  try
  {
    if (&fileInfo == this)
      return *this;

    mFileId = fileInfo.mFileId;
    mFileType = fileInfo.mFileType;
    mName = fileInfo.mName;
    mProducerId = fileInfo.mProducerId;
    mGenerationId = fileInfo.mGenerationId;
    mGroupFlags = fileInfo.mGroupFlags;
    mFlags = fileInfo.mFlags;
    mSourceId = fileInfo.mSourceId;
    mModificationTime = fileInfo.mModificationTime;
    mDeletionTime = fileInfo.mDeletionTime;

    return *this;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




std::string FileInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%s;%u;%u;%u;%u;%u;%s;%s",
        mFileId,
        mFileType,
        mName.c_str(),
        mProducerId,
        mGenerationId,
        mGroupFlags,
        mFlags,
        mSourceId,
        mModificationTime.c_str(),
        mDeletionTime.c_str());

    return std::string(st);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





std::string FileInfo::getCsvHeader()
{
  try
  {
    std::string header = "fileId;fileType;name;producerId;generationId;groupFlags;flags;sourceId;modificationTime;deletionTime";
    return header;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfo::setCsv(const char *csv)
{
  try
  {
    char *field[100];
    char st[1000];
    strcpy(st,csv);

    uint c = 0;
    field[0] = st;
    char *p = st;
    while (*p != '\0'  &&  c < 100)
    {
      if (*p == ';'  ||  *p == '\n')
      {
        *p = '\0';
        p++;
        c++;
        field[c] = p;
      }
      else
      {
        p++;
      }
    }

    if (c >= 7)
    {
      mFileId = toInt64(field[0]);
      mFileType = toInt64(field[1]);
      mName = field[2];
      mProducerId = toInt64(field[3]);
      mGenerationId = toInt64(field[4]);
      mGroupFlags = toInt64(field[5]);
      mFlags = toInt64(field[6]);
      mSourceId = toInt64(field[7]);
      if (c >= 8)
        mModificationTime = field[8];
      if (c >= 9)
        mDeletionTime = field[9];
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfo::setCsv(std::string csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





int FileInfo::compare(uint comparisonMethod,FileInfo *fileInfo)
{
  try
  {
    switch (comparisonMethod)
    {
      case ComparisonMethod::none:
        return 0;

      case ComparisonMethod::fileId:
        return uint_compare(mFileId,fileInfo->mFileId);

      case ComparisonMethod::fileName:
        return strcmp(mName.c_str(),fileInfo->mName.c_str());

      default:
        return uint_compare(mFileId,fileInfo->mFileId);
    }
    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





FileInfo* FileInfo::duplicate()
{
  try
  {
    return new FileInfo(*this);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}





void FileInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "FileInfo\n";
    stream << space(level) << "- mFileId           = " << mFileId << "\n";
    stream << space(level) << "- mFileType         = " << C_INT(mFileType) << "\n";
    stream << space(level) << "- mName             = " << mName << "\n";
    stream << space(level) << "- mProducerId       = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId     = " << mGenerationId << "\n";
    stream << space(level) << "- mGroupFlags       = " << mGroupFlags << "\n";
    stream << space(level) << "- mFlags            = " << mFlags << "\n";
    stream << space(level) << "- mSourceId         = " << mSourceId << "\n";
    stream << space(level) << "- mModificationTime = " << mModificationTime << "\n";
    stream << space(level) << "- mDeletionTime     = " << mDeletionTime << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,nullptr);
  }
}




}
}
