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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
    throw Fmi::Exception(BCP, "Operation failed!", nullptr);
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
              short startp = toInt16(numbers[0]);
              short endp = toInt16(numbers[1]);
              while (startp <= endp)
              {
                ParameterDef newdef(pdef);
                newdef.mForecastNumber = Fmi::to_string(startp);
                rec.mSourceParameters.emplace_back(newdef);
                rec.mSourceParameterNames.insert(toLowerString(newdef.getName()));
                startp++;
              }
            }
            else
            {
              rec.mSourceParameters.emplace_back(pdef);
              rec.mSourceParameterNames.insert(toLowerString(*it));
            }
          }

          rec.mFunctionName = partList[2];
          rec.mFunctionCallMethod = toUInt32(partList[3].c_str());
          splitString(partList[4],',',rec.mFunctionParameters);
          mContentDefList.emplace_back(rec);
        }
      }
    }
    fclose(file);

    mLastModified = getFileModificationTime(mFilename.c_str());
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
