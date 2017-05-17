#include "ServiceInterface.h"
#include "grid-files/common/Exception.h"


namespace SmartMet
{
namespace QueryServer
{



ServiceInterface::ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}




ServiceInterface::~ServiceInterface()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





void ServiceInterface::shutdown()
{
  try
  {
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  try
  {
    int result = _getValuesByGridPoint(sessionId,contentInfoList,coordinateType,x,y,interpolationMethod,valueList);
    return result;
  }
  catch (...)
  {
    throw SmartMet::Spine::Exception(BCP,"Operation failed!",NULL);
  }
}





int ServiceInterface::_getValuesByGridPoint(T::SessionId sessionId,T::ContentInfoList& contentInfoList,T::CoordinateType coordinateType,double x,double y,T::InterpolationMethod interpolationMethod,T::GridPointValueList& valueList)
{
  throw SmartMet::Spine::Exception(BCP,"Not implemented!");
}


}
}

