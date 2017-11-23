#include "VirtualContentFactory_type1.h"
#include "VirtualMessage.h"
#include <grid-files/common/ShowFunction.h>
#include <grid-files/common/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GribDef.h>


#define FUNCTION_TRACE FUNCTION_TRACE_OFF


namespace SmartMet
{
namespace DataServer
{


VirtualContentFactory_type1::VirtualContentFactory_type1()
{
  FUNCTION_TRACE
  try
  {
    mLastCheck = 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





VirtualContentFactory_type1::~VirtualContentFactory_type1()
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





void VirtualContentFactory_type1::init(std::string definitionFileName)
{
  FUNCTION_TRACE
  try
  {
    mContentDefinitionFile.init(definitionFileName);
    mContentDefinitionFile.print(std::cout,0,0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory_type1::addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList)
{
  FUNCTION_TRACE
  try
  {
    time_t tt = time(0);
    if ((tt - mLastCheck) > 10)
    {
      mContentDefinitionFile.checkUpdates();
      mLastCheck = tt;
    }

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      addContent(fileInfo,*contentInfo);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory_type1::addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo)
{
  FUNCTION_TRACE
  try
  {
    VirtualContentDefinition_vec contentDefinitionList;
    mContentDefinitionFile.getContentDefinitions(contentInfo.mFmiParameterName,contentDefinitionList);

    if (contentDefinitionList.size() > 0)
    {
      GRID::GridFile_sptr sourceGridFile = mGridFileManager->getFileByIdNoMapping(fileInfo.mFileId);
      if (!sourceGridFile)
      {
        //printf("*** GRID FILE NOT FOUND %u\n",fileInfo.mFileId);
        return;  // File not found
      }

      for (auto contentDef = contentDefinitionList.begin(); contentDef != contentDefinitionList.end(); ++contentDef)
      {
        //printf("***** AddContent %u\n",contentInfo.mFileId);
        //contentInfo.print(std::cout,0,0);

        Identification::ParameterDefinition_fmi_cptr def = Identification::gribDef.mMessageIdentifier_fmi.getParameterDefByName(contentDef->mTargetParameter.c_str());
        if (def != NULL)
        {

          bool found = true;
          bool registered = false;
          uint virtualFileId = 0;
          char filename[200];
          filename[0] = '\0';

          std::vector<GRID::SourceMessage> sourceMessages;
          //sourceMessages.push_back(GRID::SourceMessage(sourceGridFile,contentInfo.mMessageIndex));

          for (auto sourceParam = contentDef->mSourceParameters.begin(); sourceParam != contentDef->mSourceParameters.end() && found; ++sourceParam)
          {
            T::ContentInfoList contentList;
            T::ContentInfo *cInfo = NULL;
            if (contentDef->mSourceParameters.size() == 1)
            {
              cInfo = &contentInfo;
            }
            else
            {
              mContentServer->getContentListByParameterGenerationIdAndForecastTime(0,contentInfo.mGenerationId,T::ParamKeyType::FMI_NAME,*sourceParam,
                T::ParamLevelIdType::FMI,contentInfo.mFmiParameterLevelId,contentInfo.mParameterLevel,contentInfo.mForecastType,contentInfo.mForecastNumber,
                contentInfo.mGeometryId,contentInfo.mForecastTime,contentList);
              cInfo = contentList.getContentInfoByIndex(0);
            }

            if (cInfo == NULL)
            {
              found = false;
            }
            else
            {
              if (sourceParam == contentDef->mSourceParameters.begin())
              {
                // Checking if the virtual file is already registered to the contentServer.
                sprintf(filename,"%s-%u-%u",contentDef->mTargetParameter.c_str(),cInfo->mFileId,cInfo->mMessageIndex);

                T::FileInfo tmpFileInfo;
                if (mContentServer->getFileInfoByName(0,std::string(filename),tmpFileInfo) == 0)
                {
                  // The content is already registered.
                  registered = true;

                  GRID::GridFile_sptr vGridFile = mGridFileManager->getFileByIdNoMapping(tmpFileInfo.mFileId);
                  if (vGridFile)
                  {
                    //printf("**** File is already in the file storage %u\n",tmpFileInfo.mFileId);
                    found = false;  // File is already in the file storage
                  }
                  else
                  {
                    virtualFileId = tmpFileInfo.mFileId;
                  }
                }
              }

              GRID::GridFile_sptr sGridFile = mGridFileManager->getFileByIdNoMapping(cInfo->mFileId);
              if (!sGridFile)
                found = false;
              else
                sourceMessages.push_back(GRID::SourceMessage(sGridFile,cInfo->mMessageIndex));
            }
          }

          //if (contentDef->mSourceParameters.size() > 1)
            //printf("**** VIRTUAL CONTENT %s\n",contentDef->mTargetParameter.c_str());


          if (found  &&  filename[0] != '\0')
          {
            if (!registered)
            {
              T::FileInfo newFileInfo(fileInfo);
              newFileInfo.mName = filename;
              newFileInfo.mFileType = T::FileType::Virtual;
              newFileInfo.mFlags = fileInfo.mFlags | (uint)T::FileInfoFlags::CONTENT_VIRTUAL;

              T::ContentInfoList contentInfoList;
              T::ContentInfo *newContentInfo = new T::ContentInfo(contentInfo);

              newContentInfo->mFileId = virtualFileId;
              newContentInfo->mFileType = T::FileType::Virtual;
              newContentInfo->mMessageIndex = 0;
              newContentInfo->mFmiParameterId = def->mFmiParameterId;
              newContentInfo->mFmiParameterName = def->mParameterName;
              newContentInfo->mGribParameterId = "";
              newContentInfo->mFmiParameterUnits = def->mParameterUnits;
              newContentInfo->mGribParameterUnits = def->mParameterUnits;
              newContentInfo->mCdmParameterId = "";
              newContentInfo->mCdmParameterName = "";
              newContentInfo->mNewbaseParameterId = def->mNewbaseId;
              newContentInfo->mNewbaseParameterName = "";
              newContentInfo->mFlags = CONTENT_INFO_VIRTUAL;

              contentInfoList.addContentInfo(newContentInfo);

              if (mContentServer->addFileInfoWithContentList(0,newFileInfo,contentInfoList) == 0)
                virtualFileId = newFileInfo.mFileId;
            }

            if (virtualFileId > 0)
            {
              GRID::VirtualGridFile *virtualGridFile = new GRID::VirtualGridFile();
              virtualGridFile->setFileId(virtualFileId);

              for (auto sm = sourceMessages.begin(); sm != sourceMessages.end(); ++sm)
              {
                virtualGridFile->addPhysicalGridFile(sm->first);
                mGridFileManager->addFileUser(sm->first->getFileId(),virtualFileId);
              }

              GRID::VirtualMessage *virtualMessage = new GRID::VirtualMessage(sourceMessages);
              virtualMessage->setFunction(mFunctionCollection,mLuaFileCollection,contentDef->mFunctionName);

              virtualGridFile->addMessage(virtualMessage);

              mGridFileManager->addFile(virtualGridFile);

              //printf("AddVirtualFile %u %s\n",virtualFileId,filename);
            }
            else
            {
              printf("The virtual file has no file id. Maybe the content registration failed!\n");
            }
          }

        }
        else
        {
          printf("UNKNOWN PARAMETER : %s\n",contentDef->mTargetParameter.c_str());
        }
      }
    }
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,exception_operation_failed,NULL);
    exception.printError();
    throw exception;
  }
}



}
}
