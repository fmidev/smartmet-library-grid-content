#include "FileInfo.h"
#include <macgyver/Exception.h>
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
    mProtocol = Protocol::None;
    mServerType = ServerType::Filesys;
    mFileType = T::FileTypeValue::Unknown;
    mProducerId = 0;
    mGenerationId = 0;
    mFlags = 0;
    mSourceId = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
    mSize = 0;
    mStatus = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





FileInfo::FileInfo(const FileInfo& fileInfo)
{
  try
  {
    mFileId = fileInfo.mFileId;
    mProtocol = fileInfo.mProtocol;
    mServerType = fileInfo.mServerType;
    mServer = fileInfo.mServer;
    mFileType = fileInfo.mFileType;
    mName = fileInfo.mName;
    mProducerId = fileInfo.mProducerId;
    mGenerationId = fileInfo.mGenerationId;
    mFlags = fileInfo.mFlags;
    mSourceId = fileInfo.mSourceId;
    mModificationTime = fileInfo.mModificationTime;
    mDeletionTime = fileInfo.mDeletionTime;
    mSize = fileInfo.mSize;
    mStatus = fileInfo.mStatus;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





FileInfo::FileInfo(uint producerId,uint generationId,uchar type,const std::string& filename,uint sourceId)
{
  try
  {
    mProducerId = producerId;
    mGenerationId = generationId;
    mProtocol = Protocol::None;
    mServerType = ServerType::Filesys;
    mFileId = 0;
    mFileType = type;
    mName = filename;
    mFlags = 0;
    mSourceId = sourceId;
    mModificationTime = 0;
    mDeletionTime = 0;
    mSize = 0;
    mStatus = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





FileInfo::FileInfo(const char *csv)
{
  try
  {
    mFileId = 0;
    mProtocol = Protocol::None;
    mServerType = ServerType::Filesys;
    mFileType = T::FileTypeValue::Unknown;
    mProducerId = 0;
    mGenerationId = 0;
    mFlags = 0;
    mSourceId = 0;
    mModificationTime = 0;
    mDeletionTime = 0;
    mSize = 0;
    mStatus = 0;
    setCsv(csv);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





FileInfo::~FileInfo()
{
  try
  {
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





FileInfo& FileInfo::operator=(const FileInfo& fileInfo)
{
  try
  {
    if (&fileInfo == this)
      return *this;

    mFileId = fileInfo.mFileId;
    mProtocol = fileInfo.mProtocol;
    mServerType = fileInfo.mServerType;
    mServer = fileInfo.mServer;
    mFileType = fileInfo.mFileType;
    mName = fileInfo.mName;
    mProducerId = fileInfo.mProducerId;
    mGenerationId = fileInfo.mGenerationId;
    mFlags = fileInfo.mFlags;
    mSourceId = fileInfo.mSourceId;
    mModificationTime = fileInfo.mModificationTime;
    mDeletionTime = fileInfo.mDeletionTime;
    mSize = fileInfo.mSize;
    mStatus = fileInfo.mStatus;

    return *this;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::string FileInfo::getCsv()
{
  try
  {
    char st[1000];
    sprintf(st,"%u;%u;%s;%u;%u;%u;%u;%u;%ld;%ld;%s;%llu;%u;%u",
        mFileId,
        mFileType,
        mName.c_str(),
        mProducerId,
        mGenerationId,
        mProtocol,
        mFlags,
        mSourceId,
        mModificationTime,
        mDeletionTime,
        mServer.c_str(),
        mSize,
        mServerType,
        mStatus);

    return std::string(st);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string FileInfo::getCsvHeader()
{
  try
  {
    std::string header = "fileId;fileType;name;producerId;generationId;protocol;flags;sourceId;modificationTimeT;deletionTimeT;server;size;serverType;status";
    return header;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
      mFileId = toUInt32(field[0]);
      mFileType = toUInt8(field[1]);
      mName = field[2];
      mProducerId = toUInt32(field[3]);
      mGenerationId = toUInt32(field[4]);
      mProtocol = toUInt32(field[5]);
      mFlags = toUInt32(field[6]);
      mSourceId = toUInt32(field[7]);
      if (c >= 8)
        mModificationTime = toInt64(field[8]);
      if (c >= 9)
        mDeletionTime = toInt64(field[9]);
      if (c >= 10)
        mServer = field[10];
      if (c >= 11)
        mSize = toInt64(field[11]);
      if (c >= 12)
        mServerType = toUInt32(field[12]);
      if (c >= 13)
        mStatus = toUInt8(field[13]);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void FileInfo::setCsv(const std::string& csv)
{
  try
  {
    setCsv(csv.c_str());
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
        return num_compare(mFileId,fileInfo->mFileId);

      case ComparisonMethod::fileName:
        return strcmp(mName.c_str(),fileInfo->mName.c_str());

      default:
        return num_compare(mFileId,fileInfo->mFileId);
    }
    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void FileInfo::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "FileInfo\n";
    stream << space(level) << "- mFileId           = " << mFileId << "\n";
    stream << space(level) << "- mProtocol         = " << C_INT(mProtocol) << "\n";
    stream << space(level) << "- mServerType      = " << C_INT(mServerType) << "\n";
    stream << space(level) << "- mServer           = " << mServer << "\n";
    stream << space(level) << "- mFileType         = " << C_INT(mFileType) << "\n";
    stream << space(level) << "- mName             = " << mName << "\n";
    stream << space(level) << "- mSize             = " << mSize << "\n";
    stream << space(level) << "- mProducerId       = " << mProducerId << "\n";
    stream << space(level) << "- mGenerationId     = " << mGenerationId << "\n";
    stream << space(level) << "- mFlags            = " << mFlags << "\n";
    stream << space(level) << "- mSourceId         = " << mSourceId << "\n";
    stream << space(level) << "- mModificationTime = " << utcTimeFromTimeT(mModificationTime) << "\n";
    stream << space(level) << "- mDeletionTime     = " << utcTimeFromTimeT(mDeletionTime) << "\n";
    stream << space(level) << "- mStatus           = " << C_INT(mStatus) << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




}
}
