#pragma once

#include <string>

namespace SmartMet
{
namespace SessionManagement
{

enum Result
{
  OK                                  =  0,
  NOT_IMPLEMENTED                     = -1,
  DATA_NOT_FOUND                      = -2,
  UNEXPECTED_EXCEPTION                = -3,
  INVALID_SESSION                     = -10,
  SESSION_ATTRIBUTE_NOT_FOUND         = -11,
  SERVICE_DISABLED                    = -20,
};


std::string getResultString(int resultCode);

}
}
