#include "VirtualContentDefinitionFile.h"
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace DataServer
{


VirtualContentDefinitionFile::VirtualContentDefinitionFile()
{
  try
  {
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::VirtualContentDefinitionFile(std::string filename)
{
  try
  {
    mFilename = filename;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::VirtualContentDefinitionFile(const VirtualContentDefinitionFile& definitionList)
{
  try
  {
    mFilename = definitionList.mFilename;
    mContentDefList = definitionList.mContentDefList;
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::~VirtualContentDefinitionFile()
{
  try
  {
  }
  catch (...)
  {
    SmartMet::Spine::Exception exception(BCP,"Destructor failed",nullptr);
    exception.printError();
  }
}





void VirtualContentDefinitionFile::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    loadFile();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void VirtualContentDefinitionFile::init(std::string filename)
{
  try
  {
    mFilename = filename;
    init();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void VirtualContentDefinitionFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
      loadFile();
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




void VirtualContentDefinitionFile::getContentDefinitions(T::ContentInfo& contentInfo,std::string producerName,VirtualContentDefinition_vec& definitions)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mContentDefList.begin(); it != mContentDefList.end(); ++it)
    {
      char tmp[1000];
      char *p = tmp;
      p += sprintf(p,"%s:%s:%d:%d:%d",contentInfo.getFmiParameterName().c_str(),producerName.c_str(),contentInfo.mGeometryId,
          contentInfo.mFmiParameterLevelId,contentInfo.mParameterLevel);

      std::string s1 = toLowerString(tmp);
      std::string s2;

      if (contentInfo.mForecastNumber > 0)
      {
        p += sprintf(p,":%d:%d",contentInfo.mForecastType,contentInfo.mForecastNumber);
        s2 = toLowerString(tmp);
      }
      else
      {
        p += sprintf(p,":%d",contentInfo.mForecastType);
        s2 = toLowerString(tmp);
      }


      //if (strcasecmp("ECBSF",producerName.c_str()) == 0)
      //  printf("FIND [%s]\n",s.c_str());

      if ((contentInfo.mForecastNumber <= 0  &&  it->mSourceParameterNames.find(s1) != it->mSourceParameterNames.end()) ||
          it->mSourceParameterNames.find(s2) != it->mSourceParameterNames.end())
      {
        definitions.push_back(*it);
        //printf("--- FOUND %lu\n",definitions.size());
      }
/*
      for (auto sp = it->mSourceParameters.begin(); sp != it->mSourceParameters.end(); ++sp)
      {
        if (strcasecmp(sp->mParameterName.c_str(),contentInfo.getFmiParameterName().c_str()) == 0)
        {
          if (sp->mProducerName == "" || strcasecmp(sp->mProducerName.c_str(),producerName.c_str()) == 0)
          {
            if (sp->mGeometryId == "" || atoi(sp->mGeometryId.c_str()) == contentInfo.mGeometryId)
            {
              if (sp->mLevelId == "" || atoi(sp->mLevelId.c_str()) == contentInfo.mFmiParameterLevelId)
              {
                if (sp->mLevel == "" || atoi(sp->mLevel.c_str()) == contentInfo.mParameterLevel)
                {
                  if (sp->mForecastType == "" || atoi(sp->mForecastType.c_str()) == contentInfo.mForecastType)
                  {
                    if (sp->mForecastNumber == "" || atoi(sp->mForecastNumber.c_str()) == contentInfo.mForecastNumber)
                    {
                      definitions.push_back(*it);
                    }
                  }
                }
              }
            }
          }
        }
      }
      */
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void VirtualContentDefinitionFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"re");
    if (file == nullptr)
    {
      Spine::Exception exception(BCP,"Cannot open the definition file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mContentDefList.clear();

    char st[10000];

    while (!feof(file))
    {
      if (fgets(st,10000,file) != nullptr  &&  st[0] != '#')
      {
        std::vector<std::string> partList;
        splitString(st,';',partList);

        if (partList.size() >= 5)
        {
          VirtualContentDefinition rec;
          std::vector<std::string> params;

          rec.mVirtualParameter.set(partList[0]);
          splitString(partList[1],',',params);

          for (auto it = params.begin(); it != params.end(); ++it)
          {
            ParameterDef pdef(*it);
            std::vector<std::string> numbers;
            splitString(pdef.mForecastNumber,'-',numbers);
            if (numbers.size() == 2  &&  numbers[0] > " ")
            {
              int startp = toInt64(numbers[0]);
              int endp = toInt64(numbers[1]);
              while (startp <= endp)
              {
                ParameterDef newdef(pdef);
                newdef.mForecastNumber = Fmi::to_string(startp);
                rec.mSourceParameters.push_back(newdef);
                rec.mSourceParameterNames.insert(toLowerString(newdef.getName()));
                startp++;
              }
            }
            else
            {
              rec.mSourceParameters.push_back(pdef);
              rec.mSourceParameterNames.insert(toLowerString(*it));
            }
          }

          rec.mFunctionName = partList[2];
          rec.mFunctionCallMethod = toInt64(partList[3].c_str());
          splitString(partList[4],',',rec.mFunctionParameters);
          mContentDefList.push_back(rec);
          //rec.print(std::cout,0,0);
        }
      }
    }
    fclose(file);

    mLastModified = getFileModificationTime(mFilename.c_str());
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}





void VirtualContentDefinitionFile::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "VirtualContentDefinitionFile\n";

    for (auto it = mContentDefList.begin(); it != mContentDefList.end(); ++it)
    {
      it->print(stream,level+1,optionFlags);
    }
  }
  catch (...)
  {
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
