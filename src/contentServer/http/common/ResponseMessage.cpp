#include "contentServer/http/common/ResponseMessage.h"

#include "grid-files/common/Exception.h"
#include "grid-files/common/GeneralFunctions.h"


namespace SmartMet
{
namespace T
{


ResponseMessage::ResponseMessage()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ResponseMessage::ResponseMessage(ResponseMessage& responseMessage)
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





ResponseMessage::~ResponseMessage()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(const char *line)
{
  try
  {
    mLines.push_back(std::string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(std::string line)
{
  try
  {
    mLines.push_back(line);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ResponseMessage::addLine(unsigned long long line)
{
  try
  {
    mLines.push_back(std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(int line)
{
  try
  {
    mLines.push_back(std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(uint line)
{
  try
  {
    mLines.push_back(std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(const char *key,const char *line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + std::string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(const char *key,std::string line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + line);
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




void ResponseMessage::addLine(const char *key,unsigned long long line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(const char *key,int line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::addLine(const char *key,uint line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + std::to_string(line));
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ResponseMessage::getLineCount()
{
  try
  {
    return (uint)mLines.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ResponseMessage::getContentSize()
{
  try
  {
    uint size = 0;
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
      size = size + (uint)mLines[t].length() + 2;

    return size;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




std::string ResponseMessage::getLineByIndex(uint index)
{
  try
  {
    if (index < (uint)mLines.size())
      return mLines[index];

    return std::string("");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





long long ResponseMessage::getLineValueByIndex(uint index)
{
  try
  {
    if (index < (uint)mLines.size())
      return atoll(mLines[index].c_str());

    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





std::string ResponseMessage::getLineByKey(const char *key)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
        return std::string(s+len+1);
    }

    return std::string("");
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





long long ResponseMessage::getLineValueByKey(const char *key)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
        return atoll(s+len+1);
    }

    return 0;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool ResponseMessage::getLineByKey(const char *key,std::string& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





bool ResponseMessage::getLineByKey(const char *key,int& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (int)atoll(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




bool ResponseMessage::getLineByKey(const char *key,uint& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (uint)atoll(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




bool ResponseMessage::getLineByKey(const char *key,unsigned char& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (unsigned char)atoll(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




bool ResponseMessage::getLineByKey(const char *key,unsigned long long& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (unsigned long long)atoll(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




bool ResponseMessage::getLineByKey(const char *key,long long& value)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = (long long)atoll(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





uint ResponseMessage::getLinesByKey(const char *key,std::vector<std::string>& values)
{
  try
  {
    uint len = (uint)strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        values.push_back(std::string(s+len+1));
      }
    }

    return (uint)values.size();
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ResponseMessage::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "ResponseMessage\n";
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
      stream << space(level+2) << mLines[t] << "\n";
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}


}
}
