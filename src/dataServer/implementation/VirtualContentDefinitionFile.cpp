#include "VirtualContentDefinitionFile.h"
#include <grid-files/common/GeneralFunctions.h>


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
    throw Spine::Exception(BCP, "Operation failed!", nullptr);
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





void VirtualContentDefinitionFile::getContentDefinitions(std::string sourceParamName,std::string producerName,int geometryId,VirtualContentDefinition_vec& definitions)
{
  try
  {
    AutoThreadLock lock(&mThreadLock);

    for (auto it = mContentDefList.begin(); it != mContentDefList.end(); ++it)
    {
      if (it->mGeometryId < 0 || it->mGeometryId == geometryId)
      {
        if (it->mProducerName.length() == 0  ||  strcasecmp(it->mProducerName.c_str(),producerName.c_str()) == 0)
        {
          for (auto sp = it->mSourceParameters.begin(); sp != it->mSourceParameters.end(); ++sp)
          {
            if (strcasecmp(sp->c_str(),sourceParamName.c_str()) == 0)
            {
              definitions.push_back(*it);
            }
          }
        }
      }
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

    char st[1000];

    while (!feof(file))
    {
      if (fgets(st,1000,file) != nullptr  &&  st[0] != '#')
      {
        std::vector<std::string> partList;
        splitString(st,';',partList);

        if (partList.size() >= 5)
        {
          VirtualContentDefinition rec;

          rec.mVirtualParameter = partList[0];
          splitString(partList[1],',',rec.mSourceParameters);
          rec.mFunctionName = partList[2];
          rec.mFunctionCallMethod = toInt64(partList[3].c_str());
          rec.mProducerName = partList[4];
          if (partList[5].length() > 0)
            rec.mGeometryId = toInt64(partList[5].c_str());

          mContentDefList.push_back(rec);
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
