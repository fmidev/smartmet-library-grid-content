#include "ResponseMessage.h"
#include <macgyver/Exception.h>
#include <grid-files/common/GeneralFunctions.h>
#include <macgyver/StringConversion.h>


namespace SmartMet
{
namespace T
{


ResponseMessage::ResponseMessage()
{
  try
  {
    mLinePos = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ResponseMessage::ResponseMessage(ResponseMessage& responseMessage)
{
  try
  {
    mLinePos = 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





ResponseMessage::~ResponseMessage()
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





void ResponseMessage::addChar(char ch)
{
  try
  {
    if (ch == '\r'  || ch == '\n' ||  mLinePos == 9999)
    {
      mLine[mLinePos] = '\0';
      if (mLinePos > 0)
        addLine(mLine);

      mLinePos = 0;
    }
    else
    {
      mLine[mLinePos] = ch;
      mLinePos++;
    }
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ResponseMessage::addLine(int line)
{
  try
  {
    mLines.push_back(Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ResponseMessage::addLine(uint line)
{
  try
  {
    mLines.push_back(Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ResponseMessage::addLine(const char *key,int line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





void ResponseMessage::addLine(const char *key,uint line)
{
  try
  {
    mLines.push_back(std::string(key) + "=" + Fmi::to_string(line));
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





uint ResponseMessage::getLineCount()
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





uint ResponseMessage::getContentSize()
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




std::string ResponseMessage::getLineByIndex(uint index)
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





long long ResponseMessage::getLineValueByIndex(uint index)
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





std::string ResponseMessage::getLineByKey(const char *key)
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





int ResponseMessage::getLineValueByKey(const char *key)
{
  try
  {
    uint len = strlen(key);
    uint sz = getLineCount();
    for (uint t=0; t<sz; t++)
    {
      const char *s = mLines[t].c_str();
      if (strncasecmp(s,key,len) == 0  &&  s[len] == '=')
        return toInt32(s+len+1);
    }

    return 0;
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}





long long ResponseMessage::getLineInt64ValueByKey(const char *key)
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





bool ResponseMessage::getLineByKey(const char *key,std::string& value)
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





bool ResponseMessage::getLineByKey(const char *key,int& value)
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




bool ResponseMessage::getLineByKey(const char *key,uint& value)
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




bool ResponseMessage::getLineByKey(const char *key,unsigned char& value)
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




bool ResponseMessage::getLineByKey(const char *key,unsigned long long& value)
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




bool ResponseMessage::getLineByKey(const char *key,long long& value)
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





uint ResponseMessage::getLinesByKey(const char *key,std::vector<std::string>& values)
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
        values.push_back(std::string(s+len+1));
      }
    }

    return values.size();
  }
  catch (...)
  {
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
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
    throw Fmi::Exception(BCP,"Operation failed!",nullptr);
  }
}


}
}
