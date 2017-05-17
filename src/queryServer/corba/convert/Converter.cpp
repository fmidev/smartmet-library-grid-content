#include "Converter.h"
#include "grid-files/common/Exception.h"


namespace SmartMet
{
namespace QueryServer
{
namespace Corba
{



Converter::Converter()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





Converter::~Converter()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(T::GridPointValue& source,QueryServer::Corba::CorbaGridPointValue& target)
{
  try
  {
    target.fileId = source.mFileId;
    target.messageIndex = source.mMessageIndex;
    target.time  = CORBA::string_dup(source.mTime.c_str());
    target.value = source.mValue;
    target.x = source.mX;
    target.y = source.mY;
    target.level = source.mLevel;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridPointValue& source,T::GridPointValue& target)
{
  try
  {
    target.mFileId = source.fileId;
    target.mMessageIndex = source.messageIndex;
    target.mTime  = source.time;
    target.mValue = source.value;
    target.mX = source.x;
    target.mY = source.y;
    target.mLevel = source.level;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(T::GridPointValueList& source,QueryServer::Corba::CorbaGridPointValueList& target)
{
  try
  {
    uint len = source.getLength();
    target.length(len);
    for (uint t=0; t<len; t++)
    {
      T::GridPointValue *obj = source.getGridPointValueByIndex(t);
      QueryServer::Corba::CorbaGridPointValue corbaObject;
      convert(*obj,corbaObject);
      target[t] = corbaObject;
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void Converter::convert(QueryServer::Corba::CorbaGridPointValueList& source,T::GridPointValueList& target)
{
  try
  {
    target.clear();
    uint len = source.length();
    for (uint t=0; t<len; t++)
    {
      QueryServer::Corba::CorbaGridPointValue corbaObject = source[t];
      T::GridPointValue *obj = new T::GridPointValue();
      convert(corbaObject,*obj);
      target.addGridPointValue(obj);
    }
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}



}
}
}

