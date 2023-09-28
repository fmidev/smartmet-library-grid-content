#include "VirtualContentDefinitionFile.h"
#include <grid-files/common/GeneralFunctions.h>
#include <grid-files/identification/GridDef.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace DataServer
{


VirtualContentDefinitionFile::VirtualContentDefinitionFile()
{
  try
  {
    mLastModified = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::VirtualContentDefinitionFile(std::string filename)
{
  try
  {
    mFilename = filename;
    mLastModified = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::VirtualContentDefinitionFile(const VirtualContentDefinitionFile& definitionList)
{
  try
  {
    mFilename = definitionList.mFilename;
    mContentDefList = definitionList.mContentDefList;
    mLastModified = definitionList.mLastModified;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





VirtualContentDefinitionFile::~VirtualContentDefinitionFile()
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





void VirtualContentDefinitionFile::init()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);
    loadFile();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





bool VirtualContentDefinitionFile::checkUpdates()
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    time_t tt = getFileModificationTime(mFilename.c_str());

    if (tt != mLastModified  &&  (tt+3) < time(nullptr))
    {
      printf("*** file udated %s\n",mFilename.c_str());
      loadFile();
      return true;
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




void VirtualContentDefinitionFile::getContentDefinitions(T::ContentInfo& contentInfo,std::string producerName,VirtualContentDefinition_vec& definitions)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    char tmp[1000];
    char *p = tmp;
    p += sprintf(p,"%s:%s:%d:%d:%d",contentInfo.getFmiParameterName(),producerName.c_str(),contentInfo.mGeometryId,
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

    for (auto it = mContentDefList.begin(); it != mContentDefList.end(); ++it)
    {
      if ((contentInfo.mForecastNumber <= 0  &&  it->mSourceParameterNames.find(s1) != it->mSourceParameterNames.end()) ||
          it->mSourceParameterNames.find(s2) != it->mSourceParameterNames.end())
      {
        definitions.emplace_back(*it);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}





void VirtualContentDefinitionFile::loadFile()
{
  try
  {
    FILE *file = fopen(mFilename.c_str(),"re");
    if (file == nullptr)
    {
      Fmi::Exception exception(BCP,"Cannot open the definition file!");
      exception.addParameter("Filename",mFilename);
      throw exception;
    }

    mContentDefList.clear();

    char st[10000];

    uint line = 0;
    while (!feof(file))
    {
      if (fgets(st,10000,file) != nullptr  &&  st[0] != '#')
      {
        line++;
        std::vector<std::string> levels;
        std::vector<std::string> forecastNumbers;

        std::vector<std::string> partList;
        splitString(st,';',partList);

        if (partList.size() >= 5)
        {
          VirtualContentDefinition baseRec;

          std::vector<std::string> fields;
          splitString(partList[0],':',fields);
          int c = fields.size();

          if (c > 0)
          {
            baseRec.mVirtualParameter.mParameterName = fields[0];
            Identification::FmiParameterDef def;
            if (!Identification::gridDef.getFmiParameterDefByName(fields[0],def))
            {
              printf("### WARNING: Parameter not defined (%s). Check common parameter definitions!\n  ** File   : %s (%d)\n  ** Method : %s\n  ** Config : %s (%u)\n",fields[0].c_str(),BCP,mFilename.c_str(),line);
            }
          }

          if (c > 1)
            baseRec.mVirtualParameter.mProducerName = fields[1];

          if (c > 2)
            baseRec.mVirtualParameter.mGeometryId = fields[2];

          if (c > 3)
            baseRec.mVirtualParameter.mLevelId = fields[3];

          if (c > 4)
            splitNumbers(fields[4].c_str(),levels);

          if (c > 5)
            baseRec.mVirtualParameter.mForecastType = fields[5];

          if (c > 6)
            splitNumbers(fields[6].c_str(),forecastNumbers);

          if (c > 7)
            baseRec.mVirtualParameter.mAreaInterpolationMethod = fields[7];

          if (c > 8)
            baseRec.mVirtualParameter.mTimeInterpolationMethod = fields[8];

          if (c > 9)
            baseRec.mVirtualParameter.mLevelInterpolationMethod = fields[9];

          if (forecastNumbers.size() == 0)
            forecastNumbers.push_back("");

          std::vector<std::string> params;
          splitString(partList[1],',',params);

          for (auto level = levels.begin();level != levels.end(); ++level)
          {
            for (auto fnumber = forecastNumbers.begin();fnumber != forecastNumbers.end(); ++fnumber)
            {
              VirtualContentDefinition rec(baseRec);

              rec.mVirtualParameter.mLevel = *level;
              rec.mVirtualParameter.mForecastNumber = *fnumber;

              for (auto it = params.begin(); it != params.end(); ++it)
              {
                ParameterDef pdef(*it);

                if (pdef.mLevel.empty())
                {
                  pdef.mLevel = *level;
                }

                if (pdef.mForecastNumber.empty())
                {
                  pdef.mForecastNumber = *fnumber;
                  rec.mSourceParameters.emplace_back(pdef);
                  rec.mSourceParameterNames.insert(toLowerString(pdef.getName()));
                }
                else
                {
                  std::vector<std::string> numbers;
                  splitNumbers(pdef.mForecastNumber.c_str(),numbers);
                  for (auto n = numbers.begin(); n != numbers.end(); ++n)
                  {
                    ParameterDef newdef(pdef);
                    newdef.mForecastNumber = *n;
                    rec.mSourceParameters.emplace_back(newdef);
                    rec.mSourceParameterNames.insert(toLowerString(newdef.getName()));
                  }
                }
              }

              rec.mFunctionName = partList[2];
              rec.mFunctionCallMethod = toUInt32(partList[3].c_str());
              splitString(partList[4],',',rec.mFunctionParameters);
              mContentDefList.emplace_back(rec);
            }
          }
        }
      }
    }
    fclose(file);

    mLastModified = getFileModificationTime(mFilename.c_str());
    //print(std::cout,0,0);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
  }
}




}  // namespace QueryServer
}  // namespace SmartMet
