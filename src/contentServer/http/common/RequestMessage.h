#pragma once

#include <string>
#include <vector>

#include <grid-files/grid/Typedefs.h>



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
     void           addLine(UInt64 line);
     void           addLine(uint line);
     void           addLine(int line);
     void           addLine(const char *key,const char *line);
     void           addLine(const char *key,const std::string& line);
     void           addLine(const char *key,time_t line);
     void           addLine(const char *key,UInt64 line);
     void           addLine(const char *key,uint line);
     void           addLine(const char *key,int line);

     uint           getContentSize();
     uint           getLineCount();
     std::string    getLineByIndex(uint index);
     Int64          getLineValueByIndex(uint index);
     std::string    getLineByKey(const char *key);
     Int64          getLineValueByKey(const char *key);

     bool           getLineByKey(const char *key,std::string& value);
     bool           getLineByKey(const char *key,short& value);
     bool           getLineByKey(const char *key,int& value);
     bool           getLineByKey(const char *key,uint& value);
     bool           getLineByKey(const char *key,unsigned char& value);
     bool           getLineByKey(const char *key,UInt64 & value);
     bool           getLineByKey(const char *key,Int64 & value);
     uint           getLinesByKey(const char *key,std::vector<std::string>& values);
     void           deleteLineByIndex(uint index);

     void           print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<std::string> mLines;
};


}
}
