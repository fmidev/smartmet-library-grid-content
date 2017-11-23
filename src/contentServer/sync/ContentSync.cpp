#include "ContentSync.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/GeneralFunctions.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{

namespace ContentServer
{


ContentSync::ContentSync()
{
  FUNCTION_TRACE
  try
  {
    mSourceId = 0;
    mSessionId = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





ContentSync::~ContentSync()
{
  FUNCTION_TRACE
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::synchronize(T::SessionId sessionId,ServiceInterface *sourceInterface,ServiceInterface *targetInterface,uint sourceId,uint targetId)
{
  FUNCTION_TRACE
  try
  {
    mSessionId = sessionId;
    mSourceId = sourceId;
    mTargetId = targetId;

    readSourceProducers(sourceInterface);
    readTargetProducers(targetInterface);
    updateProducers(targetInterface);

    readSourceGenerations(sourceInterface);
    readTargetGenerations(targetInterface);
    updateGenerations(targetInterface);

    readTargetGenerations(targetInterface);
    readSourceFiles(sourceInterface);
    readTargetFiles(targetInterface);
    readSourceContent(sourceInterface);
    mSourceContentList.sort(T::ContentInfo::ComparisonMethod::file_message);
    mSourceFileList.sort(T::FileInfo::ComparisonMethod::fileName);
    mTargetFileList.sort(T::FileInfo::ComparisonMethod::fileName);
    updateFiles(targetInterface);

    readTargetFiles(targetInterface);
    readTargetContent(targetInterface);
    mTargetFileList.sort(T::FileInfo::ComparisonMethod::fileId);
    updateContent(targetInterface);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::synchronize(T::SessionId sessionId,const char *sourceDir,ServiceInterface *targetInterface,uint sourceId,uint targetId)
{
  FUNCTION_TRACE
  try
  {
    mSessionId = sessionId;
    mSourceId = sourceId;
    mTargetId = targetId;

    readSourceProducers(sourceDir);
    readTargetProducers(targetInterface);
    updateProducers(targetInterface);

    readSourceGenerations(sourceDir);
    readTargetGenerations(targetInterface);
    updateGenerations(targetInterface);

    readTargetGenerations(targetInterface);
    readSourceFiles(sourceDir);
    readTargetFiles(targetInterface);
    readSourceContent(sourceDir);
    mSourceContentList.sort(T::ContentInfo::ComparisonMethod::file_message);
    mSourceFileList.sort(T::FileInfo::ComparisonMethod::fileName);
    mTargetFileList.sort(T::FileInfo::ComparisonMethod::fileName);
    updateFiles(targetInterface);

    readTargetFiles(targetInterface);
    readTargetContent(targetInterface);
    mTargetFileList.sort(T::FileInfo::ComparisonMethod::fileId);
    updateContent(targetInterface);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceProducers(ServiceInterface *sourceInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading producers from the source data storage\n");
    mSourceProducerList.clear();
    int result = sourceInterface->getProducerInfoList(mSessionId,mSourceProducerList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the producer list from the source data storage!");
      exception.addParameter("Result",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceGenerations(ServiceInterface *sourceInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading generations from the source data storage\n");
    mSourceGenerationList.clear();
    int result = sourceInterface->getGenerationInfoList(mSessionId,mSourceGenerationList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the generation list from the source data storage!");
      exception.addParameter("Result",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceFiles(ServiceInterface *sourceInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading files from the source data storage\n");
    mSourceFileList.clear();
    uint len = 1;
    uint startFileId = 0;
    uint maxRecords = 10000;
    while (len > 0)
    {
      T::FileInfoList fileList;
      int result = sourceInterface->getFileInfoListBySourceId(mSessionId,mSourceId,startFileId,maxRecords,fileList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the file list from the source data storage!");
        exception.addParameter("Result",getResultString(result));
        throw exception;
      }

      len = fileList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *fileInfo = fileList.getFileInfoByIndex(t);
        mSourceFileList.addFileInfo(fileInfo->duplicate());
        if (fileInfo->mFileId >= startFileId)
          startFileId = fileInfo->mFileId + 1;
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceContent(ServiceInterface *sourceInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading content from the source data storage\n");
    mSourceContentList.clear();
    uint len = 1;
    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint maxRecords = 10000;

    while (len > 0)
    {
      T::ContentInfoList contentList;
      int result = sourceInterface->getContentListBySourceId(mSessionId,mSourceId,startFileId,startMessageIndex,maxRecords,contentList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the content list from the source data storage!");
        exception.addParameter("Result",getResultString(result));
        throw exception;
      }

      len = contentList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(t);
        mSourceContentList.addContentInfo(contentInfo->duplicate());
        if (contentInfo->mFileId > startFileId  || (contentInfo->mFileId == startFileId  &&  contentInfo->mMessageIndex >= startMessageIndex))
        {
          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceProducers(const char *sourceDir)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading producers from the file system\n");
    mSourceProducerList.clear();

    char filename[300];
    sprintf(filename,"%s/producers.csv",sourceDir);
    FILE *file = fopen(filename,"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL)
      {
        if (st[0] != '#')
        {
          T::ProducerInfo *info = new T::ProducerInfo();
          info->setCsv(st);
          mSourceProducerList.addProducerInfo(info);
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceGenerations(const char *sourceDir)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading generations from the file system\n");
    mSourceGenerationList.clear();

    char filename[300];
    sprintf(filename,"%s/generations.csv",sourceDir);
    FILE *file = fopen(filename,"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL)
      {
        if (st[0] != '#')
        {
          T::GenerationInfo *info = new T::GenerationInfo();
          info->setCsv(st);
          mSourceGenerationList.addGenerationInfo(info);
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceFiles(const char *sourceDir)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading files from the file system\n");
    mSourceFileList.clear();

    char filename[300];
    sprintf(filename,"%s/files.csv",sourceDir);
    FILE *file = fopen(filename,"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL)
      {
        if (st[0] != '#')
        {
          T::FileInfo *info = new T::FileInfo();
          info->setCsv(st);
          mSourceFileList.addFileInfo(info);
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readSourceContent(const char *sourceDir)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading content from the file system\n");
    mSourceContentList.clear();

    char filename[300];
    sprintf(filename,"%s/content.csv",sourceDir);
    FILE *file = fopen(filename,"r");
    if (file == NULL)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot open the file!");
      exception.addParameter("Filename",filename);
      throw exception;
    }

    char st[1000];
    while (!feof(file))
    {
      if (fgets(st,1000,file) != NULL)
      {
        if (st[0] != '#')
        {
          T::ContentInfo *info = new T::ContentInfo();
          info->setCsv(st);
          mSourceContentList.addContentInfo(info);
        }
      }
    }
    fclose(file);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readTargetProducers(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading producers from the target data storage\n");
    mTargetProducerList.clear();
    int result = targetInterface->getProducerInfoList(mSessionId,mTargetProducerList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the producer list from the target data storage!");
      exception.addParameter("Result",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readTargetGenerations(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading generations from the target data storage\n");
    mTargetGenerationList.clear();
    int result = targetInterface->getGenerationInfoList(mSessionId,mTargetGenerationList);
    if (result != 0)
    {
      SmartMet::Spine::Exception exception(BCP,"Cannot read the generation list from the target data storage!");
      exception.addParameter("Result",getResultString(result));
      throw exception;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readTargetFiles(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading files from the target data storage\n");
    mTargetFileList.clear();
    uint len = 1;
    uint startFileId = 0;
    uint maxRecords = 10000;

    while (len > 0)
    {
      T::FileInfoList fileList;
      int result = targetInterface->getFileInfoList(mSessionId,startFileId,maxRecords,fileList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the file list from the target data storage!");
        exception.addParameter("Result",getResultString(result));
        throw exception;
      }

      len = fileList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::FileInfo *fileInfo = fileList.getFileInfoByIndex(t);
        mTargetFileList.addFileInfo(fileInfo->duplicate());
        if (fileInfo->mFileId >= startFileId)
          startFileId = fileInfo->mFileId + 1;

      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::readTargetContent(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Reading content from the target data storage\n");
    mTargetContentList.clear();
    uint len = 1;
    uint startFileId = 0;
    uint startMessageIndex = 0;
    uint maxRecords = 10000;

    while (len > 0)
    {
      T::ContentInfoList contentList;
      int result = targetInterface->getContentListBySourceId(mSessionId,mTargetId,startFileId,startMessageIndex,maxRecords,contentList);
      if (result != 0)
      {
        SmartMet::Spine::Exception exception(BCP,"Cannot read the content list from the target data storage!");
        exception.addParameter("Result",getResultString(result));
        throw exception;
      }

      len = contentList.getLength();
      for (uint t=0; t<len; t++)
      {
        T::ContentInfo *contentInfo = contentList.getContentInfoByIndex(t);
        mTargetContentList.addContentInfo(contentInfo->duplicate());
        if (contentInfo->mFileId > startFileId  || (contentInfo->mFileId == startFileId  &&  contentInfo->mMessageIndex >= startMessageIndex))
        {
          startFileId = contentInfo->mFileId;
          startMessageIndex = contentInfo->mMessageIndex + 1;
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::updateProducers(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Updating producers\n");
    uint len = mTargetProducerList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *targetProducer = mTargetProducerList.getProducerInfoByIndex(t);
      if (targetProducer->mSourceId == mTargetId)
      {
        T::ProducerInfo *sourceProducer = mSourceProducerList.getProducerInfoByName(targetProducer->mName);
        if (sourceProducer == NULL)
        {
          // The producer information is not available in the source data storage. So, we should remove
          // it also from the target data storage.

          printf("- Remove producer %s\n",targetProducer->mName.c_str());

          int result = targetInterface->deleteProducerInfoById(mSessionId,targetProducer->mProducerId);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP,"Cannot delete the producer information from the target data storage!");
            exception.addParameter("ProducerId",std::to_string(targetProducer->mProducerId));
            exception.addParameter("ProducerName",targetProducer->mName);
            exception.addParameter("Result",getResultString(result));
            throw exception;
          }
        }
      }
    }


    len = mSourceProducerList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ProducerInfo *sourceProducer = mSourceProducerList.getProducerInfoByIndex(t);
      if (sourceProducer->mSourceId == mSourceId)
      {
        T::ProducerInfo *targetProducer = mTargetProducerList.getProducerInfoByName(sourceProducer->mName);
        if (targetProducer == NULL)
        {
          // The producer information is not available in the target data storage. So, we should add it.

          T::ProducerInfo producer(*sourceProducer);
          producer.mProducerId = 0;
          producer.mSourceId = mTargetId;

          printf("- Add producer %s\n",producer.mName.c_str());

          int result = targetInterface->addProducerInfo(mSessionId,producer);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP,"Cannot add the producer information into the target data storage!");
            exception.addParameter("ProducerId",std::to_string(sourceProducer->mProducerId));
            exception.addParameter("ProducerName",sourceProducer->mName);
            exception.addParameter("Result",getResultString(result));
            throw exception;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::updateGenerations(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Updating generations\n");
    uint len = mTargetGenerationList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *targetGeneration = mTargetGenerationList.getGenerationInfoByIndex(t);
      if (targetGeneration->mSourceId == mTargetId)
      {
        T::GenerationInfo *sourceGeneration = mSourceGenerationList.getGenerationInfoByName(targetGeneration->mName);
        if (sourceGeneration == NULL)
        {
          // The generation information is not available in the source data storage. So, we should remove
          // it also from the target data storage.

          printf("- Remove generation %u %s\n",targetGeneration->mGenerationId,targetGeneration->mName.c_str());

          int result = targetInterface->deleteGenerationInfoById(mSessionId,targetGeneration->mGenerationId);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP,"Cannot delete the generation information from the target data storage!");
            exception.addParameter("GenerationId",std::to_string(targetGeneration->mGenerationId));
            exception.addParameter("GenerationName",targetGeneration->mName);
            exception.addParameter("Result",getResultString(result));
            throw exception;
          }
        }
      }
    }

    len = mSourceGenerationList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::GenerationInfo *sourceGeneration = mSourceGenerationList.getGenerationInfoByIndex(t);
      if (sourceGeneration->mSourceId == mSourceId)
      {
        T::GenerationInfo *targetGeneration = mTargetGenerationList.getGenerationInfoByName(sourceGeneration->mName);
        if (targetGeneration == NULL)
        {
          // The generation information is not available in the target data storage. So, we should add it.

          // Finding producer name:
          T::ProducerInfo *sourceProducer = mSourceProducerList.getProducerInfoById(sourceGeneration->mProducerId);
          if (sourceProducer != NULL)
          {
            // Finding producer id from the target data storage.
            T::ProducerInfo targetProducer;
            int result = targetInterface->getProducerInfoByName(mSessionId,sourceProducer->mName,targetProducer);
            if (result != 0)
            {
              SmartMet::Spine::Exception exception(BCP,"The producer information not found from the target data storage!");
              exception.addParameter("ProducerName",sourceProducer->mName);
              exception.addParameter("Result",getResultString(result));
              throw exception;
            }

            T::GenerationInfo generationInfo(*sourceGeneration);
            generationInfo.mProducerId = targetProducer.mProducerId;
            generationInfo.mSourceId = mTargetId;

            printf("- Add generation %s\n",generationInfo.mName.c_str());
            result = targetInterface->addGenerationInfo(mSessionId,generationInfo);
            if (result != 0)
            {
              SmartMet::Spine::Exception exception(BCP,"Cannot add the generation information into the target data storage!");
              exception.addParameter("GenerationName",generationInfo.mName);
              exception.addParameter("Result",getResultString(result));
              throw exception;
            }
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::updateFiles(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Updating files\n");
    uint len = mTargetFileList.getLength();
    for (uint t=0; t<len; t++)
    {
      //if ((t % 1000) == 0)
        //printf("* remove %u\n",t);

      T::FileInfo *targetFile = mTargetFileList.getFileInfoByIndex(t);
      if (targetFile->mSourceId == mTargetId)
      {
        T::FileInfo *sourceFile = mSourceFileList.getFileInfoByName(targetFile->mName);
        if (sourceFile == NULL)
        {
          // The file information is not available in the source data storage. So, we should remove
          // it also from the target data storage.

          printf("- Remove file %s\n",targetFile->mName.c_str());

          int result = targetInterface->deleteFileInfoById(mSessionId,targetFile->mFileId);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP,"Cannot delete the generation information from the target data storage!");
            exception.addParameter("FileId",std::to_string(targetFile->mFileId));
            exception.addParameter("FileName",targetFile->mName);
            exception.addParameter("Result",getResultString(result));
            throw exception;
          }
        }
      }
    }

    len = mSourceFileList.getLength();
    for (uint t=0; t<len; t++)
    {
      //if ((t % 1000) == 0)
        //printf("* add %u\n",t);

      T::FileInfo *sourceFile = mSourceFileList.getFileInfoByIndex(t);
      if (sourceFile->mSourceId == mSourceId)
      {
        T::FileInfo *targetFile = mTargetFileList.getFileInfoByName(sourceFile->mName);
        if (targetFile == NULL)
        {
          //printf("Target file not found\n");
          //sourceFile->print(std::cout,0,0);

          // The file information is not available in the target data storage. So, we should add it.

          // Finding generation name:
          T::GenerationInfo *sourceGeneration = mSourceGenerationList.getGenerationInfoById(sourceFile->mGenerationId);
          if (sourceGeneration != NULL)
          {
            // Finding producer id and generation id from the target data storage.
            T::GenerationInfo *targetGeneration = mTargetGenerationList.getGenerationInfoByName(sourceGeneration->mName);
            if (targetGeneration != NULL)
            {
              T::FileInfo fileInfo(*sourceFile);
              fileInfo.mProducerId = targetGeneration->mProducerId;
              fileInfo.mGenerationId = targetGeneration->mGenerationId;
              fileInfo.mSourceId = mTargetId;
              fileInfo.mFileId = 0;

              T::ContentInfoList contentInfoList;
              mSourceContentList.getContentInfoListByFileId(sourceFile->mFileId,contentInfoList);
              uint cLen = contentInfoList.getLength();
              for (uint c=0; c<cLen; c++)
              {
                T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(c);
                contentInfo->mProducerId = targetGeneration->mProducerId;
                contentInfo->mGenerationId = targetGeneration->mGenerationId;
                contentInfo->mSourceId = mTargetId;
                contentInfo->mFileId = 0;
              }

              printf("- Add file %s\n",fileInfo.mName.c_str());
              int result = targetInterface->addFileInfoWithContentList(mSessionId,fileInfo,contentInfoList);
              //result = targetInterface->addFileInfo(sessionId,fileInfo);
              if (result != 0)
              {
                SmartMet::Spine::Exception exception(BCP,"Cannot add the file information into the target data storage!");
                exception.addParameter("FileName",fileInfo.mName);
                exception.addParameter("Result",getResultString(result));
                throw exception;
              }
            }
          }
        }
        else
        {
          if (targetFile->mSourceId != mTargetId)
          {
            printf("- The file (%s) is already added by another source (%u)!\n",sourceFile->mName.c_str(),targetFile->mSourceId);
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void ContentSync::updateContent(ServiceInterface *targetInterface)
{
  FUNCTION_TRACE
  try
  {
    printf("Updating content\n");
    uint len = mTargetContentList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *targetContent = mTargetContentList.getContentInfoByIndex(t);
      if (targetContent->mSourceId == mTargetId)
      {
        T::FileInfo *targetFile = mTargetFileList.getFileInfoById(targetContent->mFileId);
        if (targetFile == NULL)
        {
          // The content information belongs to a file that is not available in the target data storage.
          // So, we should remove it.

          printf("- Remove content %u %u\n",targetContent->mFileId,targetContent->mMessageIndex);

          int result = targetInterface->deleteContentInfo(mSessionId,targetContent->mFileId,targetContent->mMessageIndex);
          if (result != 0)
          {
            SmartMet::Spine::Exception exception(BCP,"Cannot delete the generation information from the target data storage!");
            exception.addParameter("FileId",std::to_string(targetFile->mFileId));
            exception.addParameter("FileName",targetFile->mName);
            exception.addParameter("Result",getResultString(result));
            throw exception;
          }
        }
      }
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}


}
}
