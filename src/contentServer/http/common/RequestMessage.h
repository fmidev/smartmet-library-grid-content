#pragma once

#include <string>
#include <vector>



namespace SmartMet
{
namespace T
{


class RequestMessage
{
  public:
                    RequestMessage();
                    RequestMessage(RequestMessage& requestMessage);
     virtual        ~RequestMessage();

     void           addLine(const char *line);
     void           addLine(const std::string& line);
     void           addLine(unsigned long long line);
     void           addLine(uint line);
     void           addLine(int line);
     void           addLine(const char *key,const char *line);
     void           addLine(const char *key,const std::string& line);
     void           addLine(const char *key,unsigned long long line);
     void           addLine(const char *key,uint line);
     void           addLine(const char *key,int line);

     uint           getContentSize();
     uint           getLineCount();
     std::string    getLineByIndex(uint index);
     long long      getLineValueByIndex(uint index);
     std::string    getLineByKey(const char *key);
     long long      getLineValueByKey(const char *key);

     bool           getLineByKey(const char *key,std::string& value);
     bool           getLineByKey(const char *key,short& value);
     bool           getLineByKey(const char *key,int& value);
     bool           getLineByKey(const char *key,uint& value);
     bool           getLineByKey(const char *key,unsigned char& value);
     bool           getLineByKey(const char *key,unsigned long long& value);
     bool           getLineByKey(const char *key,long long& value);
     uint           getLinesByKey(const char *key,std::vector<std::string>& values);
     void           deleteLineByIndex(uint index);

     void           print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<std::string> mLines;
};


}
}
