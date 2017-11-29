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
    //mContentDefinitionFile.print(std::cout,0,0);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory_type1::addFile(T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
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
      addContent(fileInfo,*contentInfo,gridFileMap);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,exception_operation_failed,NULL);
  }
}





void VirtualContentFactory_type1::addContent(T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap)
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

          bool componentsFound = true;
          bool fileExists = false;
          uint virtualFileId = 0;
          char filename[200];
          filename[0] = '\0';

          std::vector<GRID::SourceMessage> sourceMessages;
          //sourceMessages.push_back(GRID::SourceMessage(sourceGridFile,contentInfo.mMessageIndex));

          for (auto sourceParam = contentDef->mSourceParameters.begin(); sourceParam != contentDef->mSourceParameters.end() && componentsFound; ++sourceParam)
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
              componentsFound = false;
            }
            else
            {
              if (sourceParam == contentDef->mSourceParameters.begin())
              {
                // Checking if the virtual file is already registered to the contentServer.
                sprintf(filename,"VIRT-%s-%u-%u",contentDef->mTargetParameter.c_str(),cInfo->mFileId,cInfo->mMessageIndex);

                if (gridFileMap.find(std::string(filename)) != gridFileMap.end())
                {
                  fileExists = true;
                }
                else
                {
                  T::FileInfo tmpFileInfo;
                  if (mContentServer->getFileInfoByName(0,std::string(filename),tmpFileInfo) == 0)
                  {
                    // The content is already registered.
                    GRID::GridFile_sptr vGridFile = mGridFileManager->getFileByIdNoMapping(tmpFileInfo.mFileId);
                    if (vGridFile)
                    {
                      //printf("**** File is already in the file storage %u\n",tmpFileInfo.mFileId);
                      fileExists = true;  // File is already in the file storage
                    }
                    else
                    {
                      virtualFileId = tmpFileInfo.mFileId;
                    }
                  }
                }
              }

              GRID::GridFile_sptr sGridFile = mGridFileManager->getFileByIdNoMapping(cInfo->mFileId);
              if (!sGridFile)
                componentsFound = false;
              else
                sourceMessages.push_back(GRID::SourceMessage(sGridFile,cInfo->mMessageIndex));
            }
          }

          //if (contentDef->mSourceParameters.size() > 1)
            //printf("**** VIRTUAL CONTENT %s\n",contentDef->mTargetParameter.c_str());


          if (!fileExists &&  componentsFound  &&  filename[0] != '\0')
          {
            GRID::VirtualGridFile *virtualGridFile = new GRID::VirtualGridFile();
            virtualGridFile->setFileName(filename);
            virtualGridFile->setFileId(virtualFileId);
            virtualGridFile->setProducerId(contentInfo.mProducerId);
            virtualGridFile->setGenerationId(contentInfo.mGenerationId);
            virtualGridFile->setSourceId(contentInfo.mSourceId);

            for (auto sm = sourceMessages.begin(); sm != sourceMessages.end(); ++sm)
              virtualGridFile->addPhysicalGridFile(sm->first);

            GRID::VirtualMessage *virtualMessage = new GRID::VirtualMessage(sourceMessages);
            virtualMessage->setFunction(mFunctionCollection,mLuaFileCollection,contentDef->mFunctionName);

            virtualGridFile->addMessage(virtualMessage);

            T::ContentInfo *newContentInfo = virtualMessage->getContentInfo();

            newContentInfo->mFileId = 0;
            newContentInfo->mFileType = T::FileType::Virtual;
            newContentInfo->mMessageIndex = 0;
            newContentInfo->mProducerId = contentInfo.mProducerId;
            newContentInfo->mGenerationId = contentInfo.mGenerationId;
            newContentInfo->mGroupFlags = contentInfo.mGroupFlags;
            newContentInfo->mForecastTime = contentInfo.mForecastTime;
            newContentInfo->mFmiParameterId = def->mFmiParameterId;
            newContentInfo->mFmiParameterName = def->mParameterName;
            newContentInfo->mGribParameterId = "";
            newContentInfo->mCdmParameterId = "";
            newContentInfo->mCdmParameterName = "";
            newContentInfo->mNewbaseParameterId = def->mNewbaseId;
            newContentInfo->mNewbaseParameterName = "";
            newContentInfo->mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;
            newContentInfo->mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
            newContentInfo->mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;
            newContentInfo->mParameterLevel = contentInfo.mParameterLevel;
            newContentInfo->mFmiParameterUnits = def->mParameterUnits;
            newContentInfo->mGribParameterUnits = def->mParameterUnits;
            newContentInfo->mForecastType = contentInfo.mForecastType;
            newContentInfo->mForecastNumber = contentInfo.mForecastNumber;
            newContentInfo->mServerFlags = contentInfo.mServerFlags;
            newContentInfo->mFlags = CONTENT_INFO_VIRTUAL;
            newContentInfo->mSourceId = contentInfo.mSourceId;
            newContentInfo->mGeometryId = contentInfo.mGeometryId;
            newContentInfo->mModificationTime = contentInfo.mModificationTime;

            gridFileMap.insert(std::pair<std::string,GRID::VirtualGridFilePtr>(std::string(filename),virtualGridFile));
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