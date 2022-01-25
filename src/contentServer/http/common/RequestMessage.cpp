#include "RequestMessage.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace T
{


RequestMessage::RequestMessage()
{
  try
  {
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





RequestMessage::RequestMessage(RequestMessage& requestMessage)
{
  try
  {
    mLines = requestMessage.mLines;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





RequestMessage::~RequestMessage()
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





void RequestMessage::addLine(const char *line)
{
  try
  {
    mLines.emplace_back(std::string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const std::string& line)
{
  try
  {
    mLines.emplace_back(line);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void RequestMessage::addLine(unsigned long long line)
{
  try
  {
    mLines.emplace_back(std::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(int line)
{
  try
  {
    mLines.emplace_back(Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(uint line)
{
  try
  {
    mLines.emplace_back(Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const char *key,const char *line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + std::string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const char *key,const std::string& line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + line);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




void RequestMessage::addLine(const char *key,time_t line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + std::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const char *key,unsigned long long line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + std::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const char *key,int line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::addLine(const char *key,uint line)
{
  try
  {
    mLines.emplace_back(std::string(key) + "=" + Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint RequestMessage::getContentSize()
{
  try
  {
    uint size = 0;
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
      size = size + mLines[t].length() + 2;

    return size;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




uint RequestMessage::getLineCount()
{
  try
  {
    return mLines.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




std::string RequestMessage::getLineByIndex(uint index)
{
  try
  {
    if (index < mLines.size())
      return mLines[index];

    return std::string("");
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





long long RequestMessage::getLineValueByIndex(uint index)
{
  try
  {
    if (index < mLines.size())
      return toInt64(mLines[index].c_str());

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





std::string RequestMessage::getLineByKey(const char *key)
{
  try
  {
    uint len = strlen(key);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





long long RequestMessage::getLineValueByKey(const char *key)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
        return toInt64(s+len+1);
    }

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool RequestMessage::getLineByKey(const char *key,std::string& value)
{
  try
  {
    uint len = strlen(key);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





bool RequestMessage::getLineByKey(const char *key,short& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toInt16(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool RequestMessage::getLineByKey(const char *key,int& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toInt32(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool RequestMessage::getLineByKey(const char *key,uint& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toUInt32(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool RequestMessage::getLineByKey(const char *key,unsigned char& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toUInt8(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool RequestMessage::getLineByKey(const char *key,unsigned long long& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toUInt64(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}




bool RequestMessage::getLineByKey(const char *key,long long& value)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        value = toInt64(s+len+1);
        return true;
      }
    }

    return false;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint RequestMessage::getLinesByKey(const char *key,std::vector<std::string>& values)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
      {
        values.emplace_back(std::string(s+len+1));
      }
    }

    return values.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::deleteLineByIndex(uint index)
{
  try
  {
    if (index < mLines.size())
      mLines.erase(mLines.begin() + index);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void RequestMessage::print(std::ostream& stream,uint level,uint optionFlags)
{
  try
  {
    stream << space(level) << "RequestMessage\n";
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
      stream << space(level+2) << mLines[t] << "\n";
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
