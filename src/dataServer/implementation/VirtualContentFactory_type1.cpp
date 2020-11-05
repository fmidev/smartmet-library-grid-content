#include "VirtualContentFactory_type1.h"
#include "VirtualMessage.h"

#include <grid-files/common/ShowFunction.h>
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>


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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    Fmi::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void VirtualContentFactory_type1::init(std::string definitionFileName)
{
  FUNCTION_TRACE
  try
  {
    mContentDefinitionFile.init(definitionFileName);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualContentFactory_type1::addFile(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfoList& contentInfoList,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    time_t tt = time(nullptr);
    if ((tt - mLastCheck) > 10)
    {
      mContentDefinitionFile.checkUpdates();
      mLastCheck = tt;
    }

    uint len = contentInfoList.getLength();
    for (uint t=0; t<len; t++)
    {
      T::ContentInfo *contentInfo = contentInfoList.getContentInfoByIndex(t);
      addContent(producerInfo,generationInfo,fileInfo,*contentInfo,gridFileMap);
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void VirtualContentFactory_type1::addContent(T::ProducerInfo& producerInfo,T::GenerationInfo& generationInfo,T::FileInfo& fileInfo,T::ContentInfo& contentInfo,VirtualGridFilePtr_map& gridFileMap)
{
  FUNCTION_TRACE
  try
  {
    VirtualContentDefinition_vec contentDefinitionList;
    mContentDefinitionFile.getContentDefinitions(contentInfo,toLowerString(producerInfo.mName),contentDefinitionList);

    time_t minDeletionTime = 0;

    if (contentDefinitionList.size() > 0)
    {
      GRID::GridFile_sptr sourceGridFile = mGridFileManager->getFileByIdNoMapping(fileInfo.mFileId);
      if (!sourceGridFile)
      {
        //printf("*** GRID FILE NOT FOUND %u\n",fileInfo.mFileId);
        return;  // File not found
      }

      time_t deletionTime = sourceGridFile->getDeletionTime();
      if (deletionTime != 0)
      {
        if ((time(nullptr) + 300) > deletionTime)
        {
          // The grid file will be deleted soon. We should not access it anymore.
          return;
        }

        if (minDeletionTime == 0 || deletionTime < minDeletionTime)
          minDeletionTime = deletionTime;
      }

      for (auto contentDef = contentDefinitionList.begin(); contentDef != contentDefinitionList.end(); ++contentDef)
      {

        char filename[300];
        filename[0] = '\0';
        sprintf(filename,"%s-%s-%s-%s-%s-%s-%s-%u-%s",
            contentDef->mVirtualParameter.mProducerName.c_str(),
            contentDef->mVirtualParameter.mParameterName.c_str(),
            contentDef->mVirtualParameter.mGeometryId.c_str(),
            contentDef->mVirtualParameter.mLevelId.c_str(),
            contentDef->mVirtualParameter.mLevel.c_str(),
            contentDef->mVirtualParameter.mForecastType.c_str(),
            contentDef->mVirtualParameter.mForecastNumber.c_str(),
            generationInfo.mGenerationId,
            contentInfo.mForecastTime.c_str());

        if (gridFileMap.find(std::string(filename)) == gridFileMap.end())
        {

          Identification::FmiParameterDef def;
          if (Identification::gridDef.getFmiParameterDefByName(contentDef->mVirtualParameter.mParameterName.c_str(),def))
          {
            bool componentsFound = true;
            bool fileExists = false;
            uint virtualFileId = 0;

            std::vector<GRID::SourceMessage> sourceMessages;

            for (auto sourceParam = contentDef->mSourceParameters.begin(); sourceParam != contentDef->mSourceParameters.end() && componentsFound; ++sourceParam)
            {
              T::ContentInfoList contentList;
              T::ContentInfo *cInfo = nullptr;
              if (contentDef->mSourceParameters.size() == 1)
              {
                cInfo = &contentInfo;
              }
              else
              {
                int levelId = contentInfo.mFmiParameterLevelId;
                if (sourceParam->mLevelId > " ")
                  levelId = atoi(sourceParam->mLevelId.c_str());

                int level = contentInfo.mParameterLevel;
                if (sourceParam->mLevel > " ")
                  level = atoi(sourceParam->mLevel.c_str());

                std::string producerName = producerInfo.mName;
                if (sourceParam->mProducerName > " ")
                  producerName = sourceParam->mProducerName;

                int forecastType = contentInfo.mForecastType;
                if (sourceParam->mForecastType > " ")
                  forecastType = atoi(sourceParam->mForecastType.c_str());

                int forecastNumber = contentInfo.mForecastNumber;
                if (sourceParam->mForecastNumber > " ")
                  forecastNumber = atoi(sourceParam->mForecastNumber.c_str());

                int geometryId = contentInfo.mGeometryId;
                if (sourceParam->mGeometryId > " ")
                  geometryId = atoi(sourceParam->mGeometryId.c_str());

                mContentServer->getContentListByParameterGenerationIdAndForecastTime(0,contentInfo.mGenerationId,T::ParamKeyTypeValue::FMI_NAME,sourceParam->mParameterName,
                  T::ParamLevelIdTypeValue::FMI,levelId,level,forecastType,forecastNumber,geometryId,contentInfo.mForecastTime,contentList);

                //mContentServer->getContentListByParameterAndProducerName(0,producerName,T::ParamKeyTypeValue::FMI_NAME,sourceParam->mParameterName,T::ParamLevelIdTypeValue::FMI,levelId,level,level,forecastType,forecastNumber,geometryId,contentInfo.mForecastTime,contentInfo.mForecastTime,0,contentList);

                uint len = contentList.getLength();
                for (uint t = 0; t<len; t++)
                {
                  T::ContentInfo *ci = contentList.getContentInfoByIndex(t);
                  if (ci->mForecastTime == contentInfo.mForecastTime)
                  {
                    if (ci->mGeometryId == contentInfo.mGeometryId)
                    {
                      T::GenerationInfo gInfo;
                      mContentServer->getGenerationInfoById(0,ci->mGenerationId,gInfo);
                      if (ci->mGenerationId == contentInfo.mGenerationId  ||  gInfo.mAnalysisTime == generationInfo.mAnalysisTime)
                      {
                        cInfo = ci;
                      }
                    }
                  }
                }
              }

              if (cInfo == nullptr || (contentList.getLength() == 1  &&  cInfo->mForecastTime != contentInfo.mForecastTime))
              {
                 // Not found
                componentsFound = false;
              }
              else
              {
                if (sourceParam == contentDef->mSourceParameters.begin())
                {
                  // Checking if the virtual file is already registered to the contentServer.

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

            if (!fileExists &&  componentsFound  &&  filename[0] != '\0')
            {
              GRID::VirtualGridFile *virtualGridFile = new GRID::VirtualGridFile();
              virtualGridFile->setFileName(filename);
              virtualGridFile->setFileId(virtualFileId);
              virtualGridFile->setProducerId(contentInfo.mProducerId);
              virtualGridFile->setGenerationId(contentInfo.mGenerationId);
              virtualGridFile->setSourceId(contentInfo.mSourceId);
              virtualGridFile->setDeletionTime(minDeletionTime);

              for (auto sm = sourceMessages.begin(); sm != sourceMessages.end(); ++sm)
                virtualGridFile->addPhysicalGridFile(sm->first);

              GRID::VirtualMessage *virtualMessage = new GRID::VirtualMessage(virtualGridFile,contentDef->mVirtualParameter,sourceMessages);
              virtualMessage->setFunction(mFunctionCollection,mLuaFileCollection,contentDef->mFunctionName,contentDef->mFunctionCallMethod,contentDef->mFunctionParameters);

              virtualGridFile->addMessage(virtualMessage);

              T::ContentInfo *newContentInfo = virtualMessage->getContentInfo();

              Identification::NewbaseParameterDef newbaseDef;
              Identification::gridDef.getNewbaseParameterDefByFmiId(def.mFmiParameterId,newbaseDef);

              newContentInfo->mFileId = 0;
              newContentInfo->mFileType = T::FileTypeValue::Virtual;
              newContentInfo->mMessageIndex = 0;
              newContentInfo->mProducerId = contentInfo.mProducerId;
              newContentInfo->mGenerationId = contentInfo.mGenerationId;
              newContentInfo->mGroupFlags = contentInfo.mGroupFlags;
              newContentInfo->mForecastTime = contentInfo.mForecastTime;
              newContentInfo->mFmiParameterId = def.mFmiParameterId;
              newContentInfo->setFmiParameterName(def.mParameterName);
              newContentInfo->mGribParameterId = "";
              newContentInfo->mCdmParameterId = "";
              newContentInfo->mCdmParameterName = "";
              newContentInfo->mNewbaseParameterId = newbaseDef.mNewbaseParameterId;
              newContentInfo->mNewbaseParameterName = newbaseDef.mParameterName;

              if (contentDef->mVirtualParameter.mLevelId > " ")
                newContentInfo->mFmiParameterLevelId = atoi(contentDef->mVirtualParameter.mLevelId.c_str());
              else
                newContentInfo->mFmiParameterLevelId = contentInfo.mFmiParameterLevelId;

              newContentInfo->mGrib1ParameterLevelId = contentInfo.mGrib1ParameterLevelId;
              newContentInfo->mGrib2ParameterLevelId = contentInfo.mGrib2ParameterLevelId;

              if (contentDef->mVirtualParameter.mLevel > " ")
                newContentInfo->mParameterLevel = atoi(contentDef->mVirtualParameter.mLevel.c_str());
              else
                newContentInfo->mParameterLevel = contentInfo.mParameterLevel;

              newContentInfo->mFmiParameterUnits = def.mParameterUnits;
              newContentInfo->mGribParameterUnits = def.mParameterUnits;

              if (contentDef->mVirtualParameter.mForecastType > " ")
                newContentInfo->mForecastType = atoi(contentDef->mVirtualParameter.mForecastType.c_str());
              else
                newContentInfo->mForecastType = contentInfo.mForecastType;

              if (contentDef->mVirtualParameter.mForecastNumber > " ")
                newContentInfo->mForecastNumber = atoi(contentDef->mVirtualParameter.mForecastNumber.c_str());
              else
                newContentInfo->mForecastNumber = contentInfo.mForecastNumber;

              newContentInfo->mServerFlags = contentInfo.mServerFlags;
              newContentInfo->mFlags = T::ContentInfo::Flags::VirtualContent;
              newContentInfo->mSourceId = contentInfo.mSourceId;
              newContentInfo->mGeometryId = contentInfo.mGeometryId;
              newContentInfo->mModificationTime = contentInfo.mModificationTime;

              gridFileMap.insert(std::pair<std::string,GRID::VirtualGridFilePtr>(std::string(filename),virtualGridFile));
            }
          }
          else
          {
            // UNKNOWN PARAMETER
          }
        }
      }
    }
  }
  catch (...)
  {
    Fmi::Exception exception(BCP,"Operation failed!",nullptr);
    exception.printError();
    throw exception;
  }
}



}
}
