#pragma once

#include <grid-files/grid/Typedefs.h>

#include <string>
#include <vector>


namespace SmartMet
{
namespace T
{

// ====================================================================================
/*! \brief Serialisable key-value message used to encode an HTTP ContentServer response.
 *
 *  Mirrors the structure of RequestMessage for the response direction.  The server
 *  side fills in result values line by line; the HTTP client then reads them back
 *  using getLineByKey() to reconstruct strongly-typed return values. */
// ====================================================================================
class ResponseMessage
{
  public:
                    ResponseMessage();
                    ResponseMessage(ResponseMessage& responseMessage);
     virtual        ~ResponseMessage();

     void           addChar(char ch);
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
     int            getLineValueByKey(const char *key);
     Int64          getLineInt64ValueByKey(const char *key);

     bool           getLineByKey(const char *key,std::string& value);
     bool           getLineByKey(const char *key,int& value);
     bool           getLineByKey(const char *key,uint& value);
     bool           getLineByKey(const char *key,unsigned char& value);
     bool           getLineByKey(const char *key,UInt64 & value);
     bool           getLineByKey(const char *key,Int64 & value);
     uint           getLinesByKey(const char *key,std::vector<std::string>& values);

     void           print(std::ostream& stream,uint level,uint optionFlags);

  protected:

     std::vector<std::string> mLines;      //!< Ordered list of serialised lines (optionally key=value formatted).

     uint           mLinePos;             //!< Current write position within mLine during character-by-character assembly.
     char           mLine[10000];         //!< Temporary buffer used while assembling an incoming line via addChar().

};


}
}
